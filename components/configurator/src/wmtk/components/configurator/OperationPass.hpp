#pragma once

#include <memory>
#include <wmtk/Scheduler.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
namespace wmtk {
namespace operations {
class Operation;
}

namespace components::configurator {
class Configurator;
class Pass;
class OperationPassOptions;

/// Defines how a single pass over an operation performs
struct OperationPass
{
    OperationPass(Configurator& c, const OperationPassOptions& o, Pass* pass = nullptr);
    OperationPass() = default;
    OperationPass& operator=(const OperationPass&) = default;
    OperationPass& operator=(OperationPass&&) = default;
    OperationPass(const OperationPass&) = default;
    OperationPass(OperationPass&&) = default;
    ~OperationPass() = default;
    OperationPass(
        Configurator& c,
        const OperationPassOptions& opts,
        std::shared_ptr<SchedulerBase> default_scheduler = nullptr);
    std::shared_ptr<wmtk::operations::Operation> m_operation;
    std::shared_ptr<SchedulerBase> m_scheduler;

    /// Runs the scheduler and consolidates the mesh
    wmtk::SchedulerStats run();
    /// Runs the scheduler and consolidates the mesh
    wmtk::SchedulerStats run(SchedulerBase& s);
}; // namespace components::configurator
} // namespace components::configurator
} // namespace wmtk
