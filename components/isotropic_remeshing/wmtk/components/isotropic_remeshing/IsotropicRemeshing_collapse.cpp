
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/invariants/MultiMeshLinkConditionInvariant.hpp>
#include <wmtk/invariants/MultiMeshMapValidInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/multimesh/MultiMeshVisitor.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/utils/Logger.hpp>
#include "IsotropicRemeshing.hpp"

#include <wmtk/operations/attribute_update/make_cast_attribute_transfer_strategy.hpp>

#include <Eigen/Geometry>
#include <wmtk/invariants/InvariantCollection.hpp>

#include <wmtk/Mesh.hpp>
#include "internal/configure_collapse.hpp"

namespace wmtk::components::isotropic_remeshing {
void IsotropicRemeshing::configure_collapse(const IsotropicRemeshingOptions& opts)
{
    wmtk::logger().debug("Configure isotropic remeshing collapse");
    wmtk::Mesh& mesh = get_attribute(opts.position_attribute).mesh();
    auto& op = m_collapse = std::make_shared<operations::EdgeCollapse>(mesh);
    internal::configure_collapse(*op, mesh, opts);

    if (m_envelope_invariants) {
        spdlog::info("Attaching envelope invariants");
        op->add_invariant(m_envelope_invariants);
    }

    for (const auto& [c,p] : opts.copied_attributes) {
        auto parent = get_attribute(p);
        auto child = get_attribute(c);
        op->set_new_attribute_strategy(child, operations::CollapseBasicStrategy::None);

        op->add_transfer_strategy(
            wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                parent,
                child));
    }
    for (const auto& transfer : m_operation_transfers) {
        spdlog::info("Adding collapse transfer for {}", transfer->handle().name());
        op->set_new_attribute_strategy(transfer->handle());
        op->add_transfer_strategy(transfer);
    }

    if (opts.collapse.priority) {
        opts.collapse.priority->assign_to(m_meshes, *op);
    }

    if (m_universal_invariants) {
        op->add_invariant(m_universal_invariants);
    }

    assert(op->attribute_new_all_configured());
}
} // namespace wmtk::components::isotropic_remeshing
