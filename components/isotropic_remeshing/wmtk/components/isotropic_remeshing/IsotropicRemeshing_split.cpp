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
void IsotropicRemeshing::configure_split()
{
    wmtk::logger().debug("Configure isotropic remeshing split");
    wmtk::Mesh& mesh = m_options.position_attribute.mesh();
    auto& op = m_split = std::make_shared<operations::EdgeSplit>(mesh);
    internal::configure_split(*op, mesh, m_options);

    if (m_options.lock_boundary && !m_options.use_for_periodic) {
        op->add_invariant(m_interior_position_invariants);
    }

    for (const auto& [child, parent] : m_options.copied_attributes) {
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
        op->set_new_attribute_strategy(transfer->handle());
        op->add_transfer_strategy(transfer);
    }
    if (m_options.split.priority) {
        m_options.split.priority->assign_to(*m_options.mesh_collection, *op);
    }


    // if (m_universal_invariants) {
    //     m_split->add_invariant(m_universal_invariants);
    // }
    assert(op->attribute_new_all_configured());
}
} // namespace wmtk::components::isotropic_remeshing
