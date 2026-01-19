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

std::shared_ptr<SchedulerBase> SchedulerOptions::create(Configurator& c) const
{
    std::shared_ptr<SchedulerBase> r;
    switch (mode) {
    default:
    case SchedulerMode::Single: {
        Mesh& m = c.get_mesh(mesh_path);
        return std::make_shared<MeshScheduler>(m);
    }
    case SchedulerMode::Color: {
        if (mesh_path.empty()) {
            return std::make_shared<ColorScheduler>(c.get_attribute(handle));
        } else {
            Mesh& m = c.get_mesh(mesh_path);
            return std::make_shared<ColorScheduler>(m, c.get_attribute(handle));
        }
    }
    case SchedulerMode::Flag:
        if (mesh_path.empty()) {
            return std::make_shared<FlagScheduler>(c.get_attribute(handle));
        } else {
            Mesh& m = c.get_mesh(mesh_path);
            return std::make_shared<FlagScheduler>(m, c.get_attribute(handle));
        }
    }
}

bool SchedulerOptions::is_valid() const
{
    switch (mode) {
    default:
    case SchedulerMode::Single: return !mesh_path.empty();
    case SchedulerMode::Color: return !handle.empty();
    case SchedulerMode::Flag: return !handle.empty();
    }
}
} // namespace wmtk::components::configurator::operations
