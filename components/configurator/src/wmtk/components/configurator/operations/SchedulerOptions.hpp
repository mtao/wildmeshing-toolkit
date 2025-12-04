#pragma once
#include <nlohmann/json_fwd.hpp>
#include "PriorityOptions.hpp"
#include "wmtk/components/multimesh/utils/AttributeDescription.hpp"

namespace wmtk {
class SchedulerBase;
}


namespace wmtk::components::configurator {
class Configurator;
}
namespace wmtk::components::configurator::operations {

enum class SchedulerMode {
    Single, //< Try running one operation on every simplex
    Color, //<
    Flag
};
struct SchedulerOptions
{
    std::string mesh_path;
    SchedulerMode mode;
    multimesh::utils::AttributeDescription handle;
    PriorityOptions priority;
    std::shared_ptr<SchedulerBase> create(const Configurator& c) const;
    bool is_valid() const;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(SchedulerOptions)
};
} // namespace wmtk::components::configurator::operations
