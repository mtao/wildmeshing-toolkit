#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Scheduler.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/utils/Logger.hpp>
#include "IsotropicRemeshing.hpp"


#include <Eigen/Geometry>
#include <wmtk/invariants/InvariantCollection.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/operations/attribute_update/make_cast_attribute_transfer_strategy.hpp>
#include "internal/configure_split.hpp"

namespace wmtk::components::isotropic_remeshing {
void IsotropicRemeshing::configure_split(const IsotropicRemeshingOptions& opts)
{
    wmtk::logger().debug("Configure isotropic remeshing split");
    wmtk::Mesh& mesh = get_attribute(opts.position_attribute).mesh();
    auto& op = m_split = std::make_shared<operations::EdgeSplit>(mesh);
    internal::configure_split(*op, mesh, opts);

    if (opts.lock_boundary) {
        op->add_invariant(m_interior_position_invariants);
    }

    if (m_envelope_invariants) {
        op->add_invariant(m_envelope_invariants);
    }
    for (const auto& [c, p] : opts.copied_attributes) {
        auto parent = get_attribute(p);
        auto child = get_attribute(c);
        op->set_new_attribute_strategy(
            child,
            operations::SplitBasicStrategy::None,
            operations::SplitRibBasicStrategy::None);

        op->add_transfer_strategy(
            wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                parent,
                child));
    }
    for (const auto& transfer : m_operation_transfers) {
        spdlog::info("Adding split transfer for {}", transfer->handle().name());
        op->set_new_attribute_strategy(transfer->handle());
        op->add_transfer_strategy(transfer);
    }
    if (opts.split.priority) {
        opts.split.priority->assign_to(m_meshes, *op);
    }


    // if (m_universal_invariants) {
    //     m_split->add_invariant(m_universal_invariants);
    // }
    assert(op->attribute_new_all_configured());
}
} // namespace wmtk::components::isotropic_remeshing
