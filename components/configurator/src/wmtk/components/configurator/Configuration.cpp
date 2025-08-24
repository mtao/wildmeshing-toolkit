#include "Configuration.hpp"
#include <nlohmann/json.hpp>

namespace wmtk::components::configurator {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(Configuration)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(input, output)
    to_json(nlohmann_json_j, static_cast<const PassConfiguration&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(Configuration)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(input, output);

    from_json(nlohmann_json_j, static_cast<PassConfiguration&>(nlohmann_json_t));
}
} // namespace wmtk::components::configurator
