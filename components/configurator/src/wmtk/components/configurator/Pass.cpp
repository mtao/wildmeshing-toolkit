#include "Pass.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/consolidate.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/utils/Logger.hpp>
#include "Configurator.hpp"
#include "wmtk/Scheduler.hpp"


namespace wmtk::components::configurator {
Pass::Pass(const Pass&) = default;
Pass::Pass(Pass&&) = default;
Pass& Pass::operator=(const Pass&) = default;
Pass& Pass::operator=(Pass&&) = default;

const Mesh& Pass::mesh() const
{
    return *m_mesh;
}

Mesh& Pass::mesh()
{
    return *m_mesh;
}

Pass::Pass(Configurator& c, const PassOptions& o)
    : m_mesh(c.get_mesh(o.mesh_path).shared_from_this())
    , m_iterations(o.iterations)
{
    for (const auto& op : o.operations) {
        m_operations.emplace_back(c.get_operation(op));
    }
}

wmtk::SchedulerStats Pass::run(std::string_view info)
{
    wmtk::Scheduler scheduler;
    SchedulerStats pass_stats;

    for (long i = 0; i < m_iterations; ++i) {
        wmtk::logger().info("Pass {}, Sub-Iteration {}", info, i);

        for (size_t j = 0; j < m_operations.size(); ++j) {
            const auto& op = m_operations[j];
            // for (const auto& op : m_operations) {
            SchedulerStats stats = scheduler.run_operation_on_all(*op, *m_mesh);
            logger().info(
                "{} Executed {} ops (S/F) {}/{}.",
                j,
                stats.number_of_performed_operations(),
                stats.number_of_successful_operations(),
                stats.number_of_failed_operations());
            pass_stats += stats;
        }

        wmtk::multimesh::consolidate(*m_mesh);
        // m_mesh.consolidate();
    }
    logger().info(
        "Executed {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, executing: {}",
        pass_stats.number_of_performed_operations(),
        pass_stats.number_of_successful_operations(),
        pass_stats.number_of_failed_operations(),
        pass_stats.collecting_time,
        pass_stats.sorting_time,
        pass_stats.executing_time);
    return pass_stats;
}

} // namespace wmtk::components::configurator
