#include "OperationPass.hpp"
#include <wmtk/multimesh/consolidate.hpp>
#include <wmtk/utils/Logger.hpp>
#include "Configurator.hpp"
#include "operations/OperationOptions.hpp"

namespace wmtk::components::configurator {

OperationPass::OperationPass(Configurator& c, const OperationPassOptions& o, Pass* pass)
    : m_operation(c.get_operation(o.operation))
{
    if (o.scheduler.is_valid()) {
        m_scheduler = o.scheduler.create(c);
    } else {
        assert(pass != nullptr);
        auto s = pass->default_scheduler();
        assert(s != nullptr);
        m_scheduler = s;
    }
}
wmtk::SchedulerStats OperationPass::run()
{
    if (!m_scheduler) {
        throw std::runtime_error("OperationPass was run without a scheduler");
    }
    return run(*m_scheduler);
}
/*
SchedulerStats stats;
if (color_mode()) {
    if (!m_handle.is_same_mesh(*m_mesh)) {
        throw std::runtime_error(
            "Color scheduling where mesh is not the right mesh for coloring");
    }
    stats = scheduler.run_operation_on_all(*op, m_handle.as<int64_t>(), *m_mesh);
    // color mode
} else if (flag_mode()) {
    if (!m_handle.is_same_mesh(*m_mesh)) {
        throw std::runtime_error(
            "Flag scheduling where mesh is not the right mesh for coloring");
    }
    stats = scheduler.run_operation_on_all(*op, m_handle.as<char8_t>(), *m_mesh);
} else {
    stats = scheduler.run_operation_on_all(*op, *m_mesh);
}
*/
wmtk::SchedulerStats OperationPass::run(SchedulerBase& s)
{
    SchedulerStats run_stats = s.run(*m_operation);

    wmtk::multimesh::consolidate(s.mesh());
    // m_mesh.consolidate();
    // logger().info(
    //    "Executed {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, executing: {}",
    //    pass_stats.number_of_performed_operations(),
    //    pass_stats.number_of_successful_operations(),
    //    pass_stats.number_of_failed_operations(),
    //    pass_stats.collecting_time,
    //    pass_stats.sorting_time,
    //    pass_stats.executing_time);
    return run_stats;
}
} // namespace wmtk::components::configurator
