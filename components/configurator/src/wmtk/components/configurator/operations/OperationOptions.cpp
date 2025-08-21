#include "OperationOptions.hpp"
// #include "PriorityOptions.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/simplex/Simplex.hpp>
#include <wmtk/utils/Logger.hpp>

namespace wmtk::components::configurator::operations {

// OperationOptions::OperationOptions() = default;
// OperationOptions::~OperationOptions() = default;

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(OperationOptions){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, enabled, priority)
    // nlohmann_json_j["priority"] = nlohmann_json_t.priority;
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(OperationOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(OperationOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, enabled, priority);

    // if (nlohmann_json_j.contains("priority")) {
    //     nlohmann_json_t.priority = nlohmann_json_j["priority"];
    // }

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
}

void EdgeSwapOptions::set_mode(EdgeSwapMode mode)
{
    std::string r;
    switch (mode) {
    case EdgeSwapMode::AMIPS: r = "amips"; break;
    case EdgeSwapMode::Valence: r = "valence"; break;
    default: break;
    }

    parameters["mode"] = r;
}
EdgeSwapMode EdgeSwapOptions::mode() const
{
    if (!parameters.contains("mode")) {
        wmtk::logger().debug("Edge swap did not have a mode, defaulting to valence");
        return EdgeSwapMode::Valence;
    }
    const std::string swap_name = parameters["mode"].get<std::string>();
    if (swap_name == "amips") {
        return EdgeSwapMode::AMIPS;
    } else if (swap_name == "valence") {
        return EdgeSwapMode::Valence;
    } else {
        throw std::runtime_error(
            fmt::format(
                "Expected edge_swap_mode to be one of [amips,valence], got [{}]",
                swap_name));
    }
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSwapOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
    nlohmann_json_t.mode();
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(VertexSmoothOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(VertexSmoothOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}


} // namespace wmtk::components::configurator::operations
