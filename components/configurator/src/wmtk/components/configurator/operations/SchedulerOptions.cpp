#include "../Configurator.hpp"

#include <wmtk/Scheduler.hpp>

namespace wmtk::components::configurator::operations {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(SchedulerOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, mode, handle, priority);
}


WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(SchedulerOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(SchedulerOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, mode, handle, priority);
}
} // namespace wmtk::components::configurator::operations
