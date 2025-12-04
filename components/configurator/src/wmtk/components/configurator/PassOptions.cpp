#include "PassOptions.hpp"
#include <nlohmann/json.hpp>

namespace wmtk::components::configurator {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(OperationPassOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(operation, scheduler)

}


WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(OperationPassOptions)
{
    if (nlohmann_json_j.is_string()) {
        nlohmann_json_t.operation = nlohmann_json_j;
    } else {
        WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(OperationPassOptions);
        WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(operation, scheduler);
    }
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PassOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, iterations, operations, default_scheduler)

}


WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PassOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(PassOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(
        mesh_path,
        iterations,
        operations,
        default_scheduler);
}

} // namespace wmtk::components::configurator
