#include "MultimeshOptions.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include "MeshCollection.hpp"
#include "wmtk/components/multimesh/axis_aligned_fusion.hpp"
namespace wmtk::components::multimesh {
void MultimeshOptions::run(MeshCollection& mc) const
{
    options->run(mc);
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type);
    nlohmann_json_t.options->from_json(nlohmann_json_j);
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshOptions)
{
    if (nlohmann_json_t.type == "boundary") {
        nlohmann_json_t.options = std::make_shared<MultimeshBoundaryOptions>();
    } else if (nlohmann_json_t.type == "tag") {
        nlohmann_json_t.options = std::make_shared<MultimeshTagOptions>();
    } else if (nlohmann_json_t.type == "bijection") {
        nlohmann_json_t.options = std::make_shared<MultimeshBijectionOptions>();
    } else if (nlohmann_json_t.type == "axis_aligned_fusion") {
        nlohmann_json_t.options = std::make_shared<MultimeshAxisAlignedFusionOptions>();
    }

    nlohmann_json_t.options->from_json(nlohmann_json_j);
}
void MultimeshOptions::to_json(nlohmann::json& j) const
{
    j = *this;
}
void MultimeshOptions::from_json(const nlohmann::json& j)
{
    *this = j;
}

} // namespace wmtk::components::multimesh
