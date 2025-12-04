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
    return m_default_scheduler->mesh();
}

Mesh& Pass::mesh()
{
    return m_default_scheduler->mesh();
}

Pass::Pass(Configurator& c, const PassOptions& o)
    : m_default_scheduler{std::make_shared<MeshScheduler>(
          c.get_mesh(o.mesh_path).shared_from_this())}
    , m_iterations(o.iterations)
{
    for (const auto& op : o.operations) {
        m_operations.emplace_back(c, op, this);
    }
}


wmtk::SchedulerStats Pass::run()
{
    SchedulerStats run_stats;
    for (size_t j = 0; j < m_operations.size(); ++j) {
        auto& op = m_operations[j];
        SchedulerStats stats = op.run();
        // for (const auto& op : m_operations) {
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
wmtk::SchedulerStats Pass::run(std::string_view info, int64_t iterations)
{
    SchedulerStats pass_stats;

    for (long i = 0; i < iterations; ++i) {
        wmtk::logger().info("Pass {}, Sub-Iteration {} of {}", info, i, iterations);

        SchedulerStats run_stats = m_operations[j]->run();
        pass_stats += run_stats;

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

int64_t Pass::iterations() const
{
    return m_iterations;
}
} // namespace wmtk::components::configurator
