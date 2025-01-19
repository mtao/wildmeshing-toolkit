#include "configure_swap.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/invariants/CannotMapSimplexInvariant.hpp>
#include <wmtk/invariants/InteriorSimplexInvariant.hpp>
#include <wmtk/invariants/SeparateSubstructuresInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/invariants/ValenceImprovementInvariant.hpp>
#include <wmtk/invariants/internal/ConstantInvariant.hpp>
#include <wmtk/invariants/uvEdgeInvariant.hpp>
#include <wmtk/operations/attribute_new/CollapseNewAttributeStrategy.hpp>
#include <wmtk/operations/attribute_new/SplitNewAttributeStrategy.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>
#include <wmtk/utils/Logger.hpp>
#include "../IsotropicRemeshingOptions.hpp"
//#include "../invariants/SwapPreserveTaggedTopologyInvariant.hpp"
#include "configure_collapse.hpp"

namespace wmtk::components::isotropic_remeshing::internal {
std::shared_ptr<wmtk::operations::Operation> tet_swap(
    TetMesh& mesh,
    const IsotropicRemeshingOptions& options);


namespace {
std::shared_ptr<wmtk::operations::composite::EdgeSwap> tri_swap(
    TriMesh& mesh,
    const IsotropicRemeshingOptions& options)
{
    auto swap = std::make_shared<wmtk::operations::composite::TriEdgeSwap>(mesh);
    swap->add_invariant(
        std::make_shared<wmtk::invariants::internal::ConstantInvariant>(mesh, true, true));
    // hack for uv
    if (options.fix_uv_seam) {
        swap->add_invariant(std::make_shared<wmtk::invariants::uvEdgeInvariant>(
            mesh,
            options.other_position_attributes.front().mesh()));
    }
    auto collapse_invars = collapse_invariants(mesh, options);

    switch (options.edge_swap_mode) {
    case EdgeSwapMode::Valence: {
        auto invariant_valence_improve =
            std::make_shared<wmtk::invariants::ValenceImprovementInvariant>(mesh);
        swap->add_invariant(invariant_valence_improve);
    }
    case EdgeSwapMode::AMIPS: {
    }
    default:
    case EdgeSwapMode::Skip: {
        assert(false);
    }
    }
    swap->collapse().add_invariant(collapse_invars);

    for (const auto& p : options.all_positions()) {
        swap->split().set_new_attribute_strategy(
            p,
            wmtk::operations::SplitBasicStrategy::None,
            wmtk::operations::SplitRibBasicStrategy::Mean);
        swap->collapse().set_new_attribute_strategy(
            p,
            wmtk::operations::CollapseBasicStrategy::CopyOther);
    }
    for (const auto& attr : options.pass_through_attributes) {
        swap->split().set_new_attribute_strategy(attr);
        swap->collapse().set_new_attribute_strategy(attr);
    }

    if (options.mesh_collection != nullptr) {
        for (const auto& mesh_name : options.static_cell_complex) {
            auto& mesh2 = options.mesh_collection->get_mesh(mesh_name);
            swap->collapse().add_invariant(
                std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(mesh, mesh2));
            switch (mesh2.top_simplex_type()) {
            case wmtk::PrimitiveType::Triangle:
                swap->add_invariant(std::make_shared<wmtk::invariants::InteriorSimplexInvariant>(
                    mesh2,
                    wmtk::PrimitiveType::Edge));
                break;
            case wmtk::PrimitiveType::Edge:
                spdlog::warn("Making the swap always go false");
                swap->add_invariant(std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                    mesh,
                    mesh2,
                    wmtk::PrimitiveType::Edge,
                    false));
                swap->add_invariant(std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                    mesh,
                    mesh2,
                    wmtk::PrimitiveType::Edge,
                    true));
                break;
            case wmtk::PrimitiveType::Vertex: break;
            case wmtk::PrimitiveType::Tetrahedron:
            default: assert(false);
            }
        }
    }
    // for (const auto& p : options.tag_attributes) {
    //     swap->split().set_new_attribute_strategy(
    //         p,
    //         wmtk::operations::SplitBasicStrategy::None,
    //         wmtk::operations::SplitRibBasicStrategy::Mean);
    //     swap->collapse().set_new_attribute_strategy(
    //         p,
    //         wmtk::operations::CollapseBasicStrategy::CopyOther);
    //     auto invar = std::make_shared<invariants::SwapPreserveTaggedTopologyInvariant>(
    //         mesh,
    //         p.as<int64_t>(),
    //         -1);
    //     swap->add_invariant(invar);
    // }
    finalize_swap(*swap, options);
    return swap;
}

} // namespace


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
    case wmtk::PrimitiveType::Tetrahedron:
    default: assert(false);
    }
}

} // namespace wmtk::components::isotropic_remeshing::internal
