#pragma once
#include <memory>
#include <vector>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include "OperationPass.hpp"
#include "PassOptions.hpp"

namespace wmtk {
class Mesh;
namespace operations {
class Operation;
}
} // namespace wmtk

namespace wmtk::components::configurator {
class Configurator;
class Pass
{
public:
    Pass(const Pass&);
    Pass(Pass&&);
    Pass& operator=(const Pass&);
    Pass& operator=(Pass&&);
    Pass(Configurator& c, const PassOptions& o);

    // info is just some info for debug logs to print things nicely
    wmtk::SchedulerStats run(std::string_view info = "");
    const std::vector<OperationPass>& operations() const { return m_operations; }

    int64_t iterations() const;
    // int64_t iterations() const { return m_iterations; }

    // runs u

    const Mesh& mesh() const;
    std::shared_ptr<SchedulerBase> default_scheduler() { return m_default_scheduler; }

    Mesh& mesh();
    //void set_operations(Configurator& c, std::span<const OperationPassOptions> op_names);


private:
    std::vector<OperationPass> m_operations;
    std::shared_ptr<SchedulerBase> m_default_scheduler;
    int64_t m_iterations;
};
} // namespace wmtk::components::configurator
