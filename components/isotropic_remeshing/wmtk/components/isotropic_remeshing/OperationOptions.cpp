#include "OperationOptions.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>

namespace wmtk::components::isotropic_remeshing {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PriorityOptions)
{
    //
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PriorityOptions)
{
    //
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(InvariantOptions){

    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type, mesh_path)}


WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(InvariantOptions)
{}


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(OperationOptions){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, enabled)

}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(OperationOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(OperationOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, enabled);

    //
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeSplitOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSplitOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeCollapseOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeCollapseOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeSwapOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
    std::string name;
    switch (nlohmann_json_t.mode) {
    case EdgeSwapMode::AMIPS: name = "amips"; break;
    case EdgeSwapMode::Valence: name = "valence"; break;
    default:
    case EdgeSwapMode::Skip: name = "skip"; break;
    }
    nlohmann_json_j["mode"] = name;
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSwapOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
    assert(nlohmann_json_j.contains("mode"));
    const std::string swap_name = nlohmann_json_j["mode"].get<std::string>();
    if (swap_name == "amips") {
        nlohmann_json_t.mode = EdgeSwapMode::AMIPS;
    } else if (swap_name == "valence") {
        nlohmann_json_t.mode = EdgeSwapMode::Valence;
    } else if (swap_name == "skip") {
        nlohmann_json_t.mode = EdgeSwapMode::Skip;
    } else {
        throw std::runtime_error(fmt::format(
            "Expected edge_swap_mode to be one of [amips,valence,skip], got [{}]",
            swap_name));
    }
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(VertexSmoothOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(VertexSmoothOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}


} // namespace wmtk::components::isotropic_remeshing
