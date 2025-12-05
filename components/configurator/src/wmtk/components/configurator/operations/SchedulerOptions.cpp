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

    std::shared_ptr<SchedulerBase> SchedulerOptions::create(const Configurator& c) const {
        switch(mode) {
            default:
            case SchedulerMode::Single:
                return std::make_shared<MeshScheduler>(c.get_mesh(mesh_path));
            case SchedulerMode::Color:
                return std::make_shared<MeshScheduler>(c.get_mesh(mesh_path));
            case SchedulerMode::Flag:
                return std::make_shared<MeshScheduler>(c.get_mesh(mesh_path));



        }
    }

bool is_valid();
} // namespace wmtk::components::configurator::operations
