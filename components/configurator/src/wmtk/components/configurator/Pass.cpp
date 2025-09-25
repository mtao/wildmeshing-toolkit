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
    if (m_iterations == 0) {
        return run_until_convergence(info);
    } else {
        return run(info, m_iterations);
    }
}

wmtk::SchedulerStats Pass::run_all_operations(Scheduler& scheduler)
{
    SchedulerStats run_stats;
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
        run_stats += stats;
    }
    return run_stats;
}
wmtk::SchedulerStats Pass::run_until_convergence(std::string_view info)
{
    wmtk::Scheduler scheduler;
    SchedulerStats pass_stats;

    for (int i = 0;; ++i) {
        wmtk::logger().info("Pass {}, Sub-Iteration {} until convergence", info, i);

        SchedulerStats run_stats = run_all_operations(scheduler);
        if (run_stats.number_of_successful_operations() == 0) {
            break;
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
wmtk::SchedulerStats Pass::run(std::string_view info, int64_t iterations)
{
    wmtk::Scheduler scheduler;
    SchedulerStats pass_stats;

    for (long i = 0; i < iterations; ++i) {
        wmtk::logger().info("Pass {}, Sub-Iteration {} of {}", info, i, iterations);

        SchedulerStats run_stats = run_all_operations(scheduler);
        pass_stats += run_stats;

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
