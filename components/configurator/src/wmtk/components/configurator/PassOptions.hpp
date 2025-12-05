#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "operations/SchedulerOptions.hpp"


namespace wmtk::components::configurator {

struct OperationPassOptions
{
    std::string operation;
    operations::SchedulerOptions scheduler = {};
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(OperationPassOptions)
    OperationPassOptions() = default;
    OperationPassOptions& operator=(const OperationPassOptions&) = default;
    OperationPassOptions& operator=(OperationPassOptions&&) = default;
    OperationPassOptions(const OperationPassOptions&) = default;
    OperationPassOptions(OperationPassOptions&&) = default;
    OperationPassOptions(const std::string& operation_name): operation(operation_name) {}
};

struct PassOptions
{
    std::string mesh_path;
    int64_t iterations = 1;
    std::vector<OperationPassOptions> operations;
    operations::SchedulerOptions default_scheduler;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PassOptions)
};


} // namespace wmtk::components::configurator
