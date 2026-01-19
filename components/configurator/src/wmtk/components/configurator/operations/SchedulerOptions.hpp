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
    Convergence,
    Color, // Convergence but parallelizing over a color attribute, only works on vertices
    Flag// convergence but keeps track of un-operated vertices
};
struct SchedulerOptions
{
    std::string mesh_path;
    SchedulerMode mode = SchedulerMode::Single;
    multimesh::utils::AttributeDescription handle = {};
    PriorityOptions priority = {};
    std::shared_ptr<SchedulerBase> create(Configurator& c) const;
    bool is_valid() const;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(SchedulerOptions)
};
} // namespace wmtk::components::configurator::operations
