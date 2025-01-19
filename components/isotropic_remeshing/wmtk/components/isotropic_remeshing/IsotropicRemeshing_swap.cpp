#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/invariants/CannotMapSimplexInvariant.hpp>
#include <wmtk/invariants/InteriorSimplexInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/invariants/ValenceImprovementInvariant.hpp>
#include <wmtk/invariants/internal/ConstantInvariant.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/operations/attribute_update/make_cast_attribute_transfer_strategy.hpp>
#include <wmtk/operations/composite/EdgeSwap.hpp>
#include <wmtk/operations/composite/TetEdgeSwap.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>
#include <wmtk/utils/Logger.hpp>
#include "IsotropicRemeshing.hpp"
#include "internal/configure_collapse.hpp"
namespace wmtk::components::isotropic_remeshing {
namespace internal {
namespace {

void configure_swap_transfer(
    operations::composite::EdgeSwap& swap,
    const attribute::MeshAttributeHandle& handle)
{
    switch (handle.primitive_type()) {
    case wmtk::PrimitiveType::Vertex: {
        swap.split().set_new_attribute_strategy(handle);
        swap.collapse().set_new_attribute_strategy(
            handle,
            wmtk::operations::CollapseBasicStrategy::CopyOther);
        return;
    }
    case wmtk::PrimitiveType::Edge:
    case wmtk::PrimitiveType::Triangle:
        swap.split().set_new_attribute_strategy(handle);
        swap.collapse().set_new_attribute_strategy(
            handle,
            wmtk::operations::CollapseBasicStrategy::CopyOther);
        break;
    case wmtk::PrimitiveType::Tetrahedron:
    default: assert(false);
    }
}
} // namespace
} // namespace internal

void IsotropicRemeshing::configure_swap()
{
    // adds common invariants like inversion check and asserts taht the swap is ready for prime time
    wmtk::logger().debug("Configure isotropic remeshing swap");
    if (m_options.swap.mode == EdgeSwapMode::Skip) {
        return;
    }
    wmtk::Mesh& mesh = m_options.position_attribute.mesh();
    switch (mesh.top_simplex_type()) {
    case PrimitiveType::Triangle:
        m_swap = std::make_shared<operations::composite::TriEdgeSwap>(static_cast<TriMesh&>(mesh));
        break;
    case PrimitiveType::Tetrahedron:
        m_swap = std::make_shared<operations::composite::TetEdgeSwap>(static_cast<TetMesh&>(mesh));
        break;
    case PrimitiveType::Vertex:
    case PrimitiveType::Edge:
    default: assert(false); break;
    }


    const std::optional<attribute::MeshAttributeHandle>& position_for_inversion =
        m_options.inversion_position_attribute;

    switch (m_options.swap.mode) {
    case EdgeSwapMode::Valence: {
        auto tri = dynamic_cast<TriMesh*>(&mesh);
        if (tri == nullptr) {
            throw std::runtime_error(
                "EdgeSwap only works with trimesh, got a different type of mesh");
        }
        auto invariant_valence_improve =
            std::make_shared<wmtk::invariants::ValenceImprovementInvariant>(*tri);
        m_swap->add_invariant(invariant_valence_improve);
        break;
    }
    case EdgeSwapMode::AMIPS: {
    }
    default:
    case EdgeSwapMode::Skip: {
        assert(false);
    }
    }
    // if (position_for_inversion) {
    //     m_swap->collapse().add_invariant(std::make_shared<SimplexInversionInvariant<double>>(
    //         position_for_inversion.value().mesh(),
    //         position_for_inversion.value().as<double>()));
    // }
    //  m_swap->add_invariant(m_interior_edge_invariants);

    // internal::configure_split(m_swap->split(), mesh, m_options);
    // internal::configure_collapse(m_swap->collapse(), mesh, m_options);
    m_swap->collapse().add_invariant(internal::collapse_core_invariants(mesh, m_options));


    if (m_envelope_invariants) {
        m_swap->add_invariant(m_envelope_invariants);
    }


    for (const auto& p : m_options.all_positions()) {
        internal::configure_swap_transfer(*m_swap, p);
    }
    // clear out all the pass through attributes
    for (const auto& attr : m_options.pass_through_attributes) {
        m_swap->split().set_new_attribute_strategy(attr);
        m_swap->collapse().set_new_attribute_strategy(attr);
    }
    for (const auto& [child, parent] : m_options.copied_attributes) {
        m_swap->split().set_new_attribute_strategy(child);
        m_swap->collapse().set_new_attribute_strategy(child);
        m_swap->add_transfer_strategy(
            wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                parent,
                child));
    }
    if (m_options.mesh_collection != nullptr) {
        if (!m_options.static_cell_complex.empty()) {
            assert(m_options.static_cell_complex.size() == mesh.top_cell_dimension());
            std::vector<std::shared_ptr<Mesh>> static_meshes;
            for (const auto& mesh_name : m_options.static_cell_complex) {
                auto& mesh2 = m_options.mesh_collection->get_mesh(mesh_name);
                static_meshes.emplace_back(mesh2.shared_from_this());
            }
            m_swap->add_invariant(std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                mesh,
                *static_meshes[1],
                wmtk::PrimitiveType::Edge,
                false));
            /*
            for (const auto& mptr : static_meshes) {
                auto& mesh2 = *mptr;
                m_swap->collapse().add_invariant(
                    std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(mesh, mesh2));
                switch (mesh2.top_simplex_type()) {
                case wmtk::PrimitiveType::Triangle:
                    m_swap->add_invariant(
                        std::make_shared<wmtk::invariants::InteriorSimplexInvariant>(
                            mesh2,
                            wmtk::PrimitiveType::Edge));
                    break;
                case wmtk::PrimitiveType::Edge:
                    m_swap->add_invariant(
                        std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                            mesh,
                            mesh2,
                            wmtk::PrimitiveType::Edge,
                            false));
                    break;
                case wmtk::PrimitiveType::Vertex: break;
                case wmtk::PrimitiveType::Tetrahedron:
                default: assert(false);
                }
            }
            */
        }
    }
    for (const auto& transfer : m_operation_transfers) {
        m_swap->split().set_new_attribute_strategy(transfer->handle());
        m_swap->split().add_transfer_strategy(transfer);
        m_swap->collapse().set_new_attribute_strategy(transfer->handle());
        m_swap->collapse().add_transfer_strategy(transfer);

    }
    assert(m_swap->split().attribute_new_all_configured());
    assert(m_swap->collapse().attribute_new_all_configured());
    // m_swap = op;
}
} // namespace wmtk::components::isotropic_remeshing
