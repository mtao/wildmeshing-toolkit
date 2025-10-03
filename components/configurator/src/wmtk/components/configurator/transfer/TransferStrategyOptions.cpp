#include "TransferStrategyOptions.hpp"
#include <nlohmann/json.hpp>
namespace wmtk::components::configurator::transfer {


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(TransferStrategyOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute, type, parameters);
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(TransferStrategyOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(TransferStrategyOptions);
    if (nlohmann_json_j.is_string()) {
        nlohmann_json_t.type = "alias";
        nlohmann_json_t.attribute = nlohmann_json_j;
        return;
    }
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(attribute, type);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(parameters);
}
} // namespace wmtk::components::configurator::transfer
