#pragma once

#include <spdlog/common.h>
#include <optional>
#include "wmtk/attribute/MeshAttributeHandle.hpp"
#include "wmtk/attribute/TypedAttributeHandle.hpp"

namespace wmtk {
class Mesh;
namespace simplex {
class Simplex;
}
namespace operations {
class Operation;
}
namespace attribute {
template <typename T>
class TypedAttributeHandle;
}

class SchedulerStats
{
public:
    /**
     * @brief Returns the number of successful operations performed by the scheduler.
     *
     * The value is reset to 0 when calling `run_operation_on_all`.
     */
    int64_t number_of_successful_operations() const { return m_num_op_success; }

    /**
     * @brief Returns the number of failed operations performed by the scheduler.
     *
     * The value is reset to 0 when calling `run_operation_on_all`.
     */
    int64_t number_of_failed_operations() const { return m_num_op_fail; }

    /**
     * @brief Returns the number of performed operations performed by the scheduler.
     *
     * The value is reset to 0 when calling `run_operation_on_all`.
     */
    int64_t number_of_performed_operations() const { return m_num_op_success + m_num_op_fail; }

    inline double total_time() const { return collecting_time + sorting_time + executing_time; }

    inline void succeed() { ++m_num_op_success; }
    inline void fail() { ++m_num_op_fail; }

    inline void operator+=(const SchedulerStats& s)
    {
        m_num_op_success += s.m_num_op_success;
        m_num_op_fail += s.m_num_op_fail;

        collecting_time += s.collecting_time;
        sorting_time += s.sorting_time;
        executing_time += s.executing_time;
    }


    double collecting_time = 0;
    double sorting_time = 0;
    double executing_time = 0;

    std::vector<SchedulerStats> sub_stats;

    double avg_sub_collecting_time() const
    {
        double res = 0;
        for (const auto& s : sub_stats) {
            res += s.collecting_time;
        }
        return res / sub_stats.size();
    }

    double avg_sub_sorting_time() const
    {
        double res = 0;
        for (const auto& s : sub_stats) {
            res += s.sorting_time;
        }
        return res / sub_stats.size();
    }

    double avg_sub_executing_time() const
    {
        double res = 0;
        for (const auto& s : sub_stats) {
            res += s.executing_time;
        }
        return res / sub_stats.size();
    }

    // private:
    int64_t m_num_op_success = 0;
    int64_t m_num_op_fail = 0;

    /// Prints the number of success/fail/performed out of some total number of operations
    void print_update_log(size_t total, spdlog::level::level_enum = spdlog::level::info) const;
};

class Scheduler
{
public:
    Scheduler();
    virtual ~Scheduler();

    // runs operation on the specified simplices
    SchedulerStats run_operation_on_all(
        operations::Operation& op,
        std::vector<simplex::Simplex>&& m);
    // runs on all all k-simplices on the mesh (where k is the op's simplex type)
    SchedulerStats run_operation_on_all(operations::Operation& op, const Mesh& m);
    // runs on all with all k-simplices on the operation's mesh(where k is the op's simplex type)
    SchedulerStats run_operation_on_all(operations::Operation& op);

    virtual SchedulerStats run(operations::Operation& op);
    ///
    SchedulerStats run_operation_on_all(
        operations::Operation& op,
        const TypedAttributeHandle<char>& flag_handle);
    SchedulerStats run_operation_on_all_coloring(
        operations::Operation& op,
        const TypedAttributeHandle<int64_t>& color_handle);

    SchedulerStats run_operation_on_all(
        operations::Operation& op,
        const TypedAttributeHandle<char>& flag_handle,
        const Mesh& m);
    SchedulerStats run_operation_on_all_coloring(
        operations::Operation& op,
        const TypedAttributeHandle<int64_t>& color_handle,
        const Mesh& m);

    const SchedulerStats& stats() const { return m_stats; }

    void set_update_frequency(std::optional<size_t>&& freq = {});

protected:
    SchedulerStats m_stats;
    std::optional<size_t> m_update_frequency = {};

    /// Logs the stats with the help of the number of simplices
    void log(const size_t total_simplices);
    /// Logs the stats with the help of the number of simplices
    void log(const SchedulerStats& stats, const size_t total_simplices);
};


// Base class for derived schedulers, who for now mostly just call public functions from other schedulers
class SchedulerBase : protected Scheduler
{
public:
    SchedulerBase();
    // runs on all with all k-simplices on the operation's mesh(where k is the op's simplex type)
    virtual SchedulerStats run(operations::Operation& op) = 0;

    virtual Mesh& mesh() = 0;
protected:
    using Scheduler::run_operation_on_all;
    using Scheduler::run_operation_on_all_coloring;
};

class MeshScheduler : public SchedulerBase
{
public:
    MeshScheduler(Mesh& mesh);
    // runs on all with all k-simplices on the operation's mesh(where k is the op's simplex type)
    SchedulerStats run(operations::Operation& op) override;

    Mesh& mesh() final override { return m_mesh; }

private:
    Mesh& m_mesh;
};

class FlagScheduler : public SchedulerBase
{
public:
    FlagScheduler(const attribute::MeshAttributeHandle& h);
    // runs on all with all k-simplices on the operation's mesh(where k is the op's simplex type)
    SchedulerStats run(operations::Operation& op) override;
    Mesh& mesh() final override { return m_handle.mesh(); }

private:
    attribute::MeshAttributeHandle m_handle;
};
class ColorScheduler : public SchedulerBase
{
public:
    ColorScheduler(const attribute::MeshAttributeHandle& h);
    // runs on all with all k-simplices on the operation's mesh(where k is the op's simplex type)
    SchedulerStats run(operations::Operation& op) override;
    Mesh& mesh() final override { return m_handle.mesh(); }

private:
    attribute::MeshAttributeHandle m_handle;
};

} // namespace wmtk
