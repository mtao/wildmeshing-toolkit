#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
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

void IsotropicRemeshing::configure_swap(const IsotropicRemeshingOptions& opts)
{
    // adds common invariants like inversion check and asserts taht the swap is ready for prime time
    wmtk::logger().debug("Configure isotropic remeshing swap");
    if (opts.swap.mode == EdgeSwapMode::Skip) {
        return;
    }
    wmtk::Mesh& mesh = get_attribute(opts.position_attribute).mesh();
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


    // const std::optional<attribute::MeshAttributeHandle>& position_for_inversion =
    //     opts.inversion_position_attribute;

    switch (opts.swap.mode) {
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

    // internal::configure_split(m_swap->split(), mesh, opts);
    // internal::configure_collapse(m_swap->collapse(), mesh, opts);

    add_core_collapse_invariants(m_swap->collapse(), opts);

    if (m_envelope_invariants) {
        // m_swap->add_invariant(m_envelope_invariants);
        m_swap->split().add_invariant(m_envelope_invariants);
        m_swap->collapse().add_invariant(m_envelope_invariants);
    }


    for (const auto& p : opts.all_positions()) {
        internal::configure_swap_transfer(*m_swap, get_attribute(p));
    }
    // clear out all the pass through attributes
    for (const auto& ad : opts.pass_through_attributes) {
        auto attr = get_attribute(ad);
        m_swap->split().set_new_attribute_strategy(attr);
        m_swap->collapse().set_new_attribute_strategy(attr);
    }
    for (const auto& [c, p] : opts.copied_attributes) {
        auto parent = get_attribute(p);
        auto child = get_attribute(c);
        m_swap->split().set_new_attribute_strategy(child);
        m_swap->collapse().set_new_attribute_strategy(child);
        m_swap->add_transfer_strategy(
            wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                parent,
                child));
    }
    if (!opts.static_meshes.empty()) {
        std::vector<std::shared_ptr<Mesh>> static_meshes;
        for (const auto& mptr :
             get_attribute(opts.position_attribute).mesh().get_all_child_meshes()) {
            // for (const auto& mesh_name : opts.static_meshes) {
            // auto& mesh2 = opts.mesh_collection->get_mesh(mesh_name);
            auto& mesh2 = *mptr;
            // static_meshes.emplace_back(mesh2.shared_from_this());
            if (mesh2.top_cell_dimension() == 1) {
                m_swap->add_invariant(
                    std::make_shared<invariants::CannotMapSimplexInvariant>(
                        mesh,
                        mesh2,
                        wmtk::PrimitiveType::Edge,
                        false));
                m_swap->add_invariant(
                    std::make_shared<invariants::CannotMapSimplexInvariant>(
                        mesh,
                        mesh2,
                        wmtk::PrimitiveType::Vertex,
                        false));
            } else if (mesh2.top_cell_dimension() == 0) {
                m_swap->add_invariant(
                    std::make_shared<invariants::CannotMapSimplexInvariant>(
                        mesh,
                        mesh2,
                        wmtk::PrimitiveType::Vertex,
                        false));
            }
        }
    }
    for (const auto& transfer : m_operation_transfers) {
        m_swap->split().set_new_attribute_strategy(transfer->handle());
        m_swap->split().add_transfer_strategy(transfer);
        m_swap->collapse().set_new_attribute_strategy(transfer->handle());
        m_swap->collapse().add_transfer_strategy(transfer);
    }
    if (opts.swap.priority) {
        opts.swap.priority->assign_to(m_meshes, *m_swap);
    }
    if (m_universal_invariants) {
        m_swap->add_invariant(m_universal_invariants);
    }
    assert(m_swap->split().attribute_new_all_configured());
    assert(m_swap->collapse().attribute_new_all_configured());
    // m_swap = op;
}
} // namespace wmtk::components::isotropic_remeshing
