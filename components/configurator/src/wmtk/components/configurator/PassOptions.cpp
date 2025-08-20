#include "PassOptions.hpp"
#include <nlohmann/json.hpp>

namespace wmtk::components::configurator {


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PassOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, iterations, operations)

}


WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PassOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(PassOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, iterations, operations)
}

} // namespace wmtk::components::configurator
