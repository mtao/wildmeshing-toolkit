#pragma once
#include <memory>
#include <vector>
#include <wmtk/Scheduler.hpp>
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
    wmtk::SchedulerStats run(std::string_view info);
    wmtk::SchedulerStats run_until_convergence(std::string_view info);
    wmtk::SchedulerStats run(std::string_view info, int64_t iterations);

    wmtk::SchedulerStats run_all_operations(Scheduler& s);
    const std::vector<std::shared_ptr<wmtk::operations::Operation>>& operations() const
    {
        return m_operations;
    }

    int64_t iterations() const { return m_iterations; }

    // runs u

    const Mesh& mesh() const;

    Mesh& mesh();

private:
    std::shared_ptr<Mesh> m_mesh;
    std::vector<std::shared_ptr<wmtk::operations::Operation>> m_operations;
    int64_t m_iterations;
};
} // namespace wmtk::components::configurator
