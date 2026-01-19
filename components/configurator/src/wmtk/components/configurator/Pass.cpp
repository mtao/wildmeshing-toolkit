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
          *c.get_mesh(o.mesh_path).shared_from_this())}
    , m_iterations(o.iterations)
{
    for (const auto& op : o.operations) {
        m_operations.emplace_back(c, op, this);
    }
}

//    void Pass::set_operations(Configurator& c, std::span<const OperationPassOptions> op_names) {
//
//    for (const auto& op : o.operations) {
//        m_operations.emplace_back(c, op, this);
//    }
//    }

wmtk::SchedulerStats Pass::run(std::string_view info)
{
    SchedulerStats pass_stats;

    spdlog::info("Pass {} has {} operations", info, m_operations.size());
    for (long i = 0; i < m_operations.size(); ++i) {
        auto& op = m_operations[i];
        wmtk::logger().info("Running Pass [{}] operation {}/{}", info, i, m_operations.size());

        SchedulerStats run_stats = op.run();
        pass_stats += run_stats;
    }
    logger().info(
        "Pass {} executed {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, executing: {}",
        info,
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
