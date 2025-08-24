
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/invariants/CannotMapSimplexInvariant.hpp>
#include <wmtk/invariants/InteriorSimplexInvariant.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/invariants/MaxEdgeLengthInvariant.hpp>
#include <wmtk/invariants/MultiMeshLinkConditionInvariant.hpp>
#include <wmtk/invariants/MultiMeshMapValidInvariant.hpp>
#include <wmtk/invariants/SeparateSubstructuresInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/invariants/uvEdgeInvariant.hpp>
#include <wmtk/multimesh/MultiMeshVisitor.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/utils/Logger.hpp>
#include "IsotropicRemeshing.hpp"
#include "wmtk/operations/attribute_new/Enums.hpp"

#include <wmtk/operations/attribute_update/make_cast_attribute_transfer_strategy.hpp>

#include <Eigen/Geometry>
#include <wmtk/invariants/InvariantCollection.hpp>

#include <wmtk/Mesh.hpp>

namespace wmtk::components::isotropic_remeshing {
namespace {}
void IsotropicRemeshing::add_core_collapse_invariants(
    wmtk::operations::EdgeCollapse& op,
    const IsotropicRemeshingOptions& opts)

{
    Mesh& mesh = op.mesh();
    // core invariants
#define NO_MULTIMESH_INVARIANTS
#if defined(NO_MULTIMESH_INVARIANTS)
    for (auto& m : op.mesh().get_multi_mesh_root().get_all_meshes()) {
        auto invariant_link_condition =
            std::make_shared<wmtk::invariants::MultiMeshLinkConditionInvariant>(*m, true);

        auto invariant_mm_map = std::make_shared<MultiMeshMapValidInvariant>(*m, true);
        op.add_invariant(invariant_link_condition);
        op.add_invariant(invariant_mm_map);
    }
#else
    auto invariant_link_condition =
        std::make_shared<wmtk::invariants::MultiMeshLinkConditionInvariant>(mesh);

    auto invariant_mm_map = std::make_shared<MultiMeshMapValidInvariant>(mesh);
    op.add_invariant(invariant_link_condition);
    op.add_invariant(invariant_mm_map);
#endif
}
void IsotropicRemeshing::configure_collapse(const IsotropicRemeshingOptions& opts)
{
    wmtk::logger().debug("Configure isotropic remeshing collapse");
    wmtk::Mesh& mesh = get_attribute(opts.position_attribute).mesh();
    auto& op = m_collapse;


    add_core_collapse_invariants(*op, opts);
    if (opts.separate_substructures) {
        auto invariant_separate_substructures =
            std::make_shared<wmtk::invariants::SeparateSubstructuresInvariant>(mesh);
        op->add_invariant(invariant_separate_substructures);
    }


    const double length_min = (4. / 5.) * opts.get_absolute_length(mesh_collection());
    auto pos_attr = get_attribute(opts.position_attribute);

    auto tmp = std::make_shared<wmtk::operations::CollapseNewAttributeStrategy<double>>(pos_attr);
    tmp->set_strategy(wmtk::operations::CollapseBasicStrategy::Mean);
    tmp->set_simplex_predicate(wmtk::operations::BasicSimplexPredicate::IsInterior);
    op->set_new_attribute_strategy(pos_attr, tmp);

    auto invariant_max_edge_length = std::make_shared<MaxEdgeLengthInvariant>(
        pos_attr.mesh(),
        pos_attr.as<double>(),
        length_min * length_min);
    op->add_invariant(invariant_max_edge_length);

    if (!opts.static_meshes.empty()) {
        for (const auto& mesh_name : opts.static_meshes) {
            auto& mesh2 = mesh_collection().get_mesh(mesh_name);
            op->add_invariant(
                std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                    mesh,
                    mesh2,
                    wmtk::PrimitiveType::Vertex));
        }
    }

    if (m_envelope_invariants) {
        spdlog::info("Attaching envelope invariants");
        op->add_invariant(m_envelope_invariants);
    }

    for (const auto& [c, p] : opts.copied_attributes) {
        auto parent = get_attribute(p);
        auto child = get_attribute(c);
        assert(parent.is_valid());
        assert(child.is_valid());
        op->set_new_attribute_strategy(child, wmtk::operations::CollapseBasicStrategy::None);

        op->add_transfer_strategy(
            wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                parent,
                child));
    }
    for (const auto& transfer : m_operation_transfers) {
        spdlog::info("Adding collapse transfer for {}", transfer->handle().name());
        op->set_new_attribute_strategy(
            transfer->handle(),
            wmtk::operations::CollapseBasicStrategy::None);
        op->add_transfer_strategy(transfer);
    }
    for (const auto& attr : opts.pass_through_attributes) {
        op->set_new_attribute_strategy(get_attribute(attr));
    }


    if (opts.get_collapse().priority) {
        opts.get_collapse().priority.assign_to(mesh_collection(), *m_collapse);
    }

    if (m_universal_invariants) {
        op->add_invariant(m_universal_invariants);
    }

    assert(op->attribute_new_all_configured());
}
} // namespace wmtk::components::isotropic_remeshing
