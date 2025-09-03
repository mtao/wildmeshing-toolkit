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

void OperationOptions::add_alias_invariant(std::string_view name)
{
    return add_alias_invariant(name, name);
}
// adds a new alias invariant for "original_name" but giving the name "name"
void OperationOptions::add_alias_invariant(std::string_view name, std::string_view original_name)
{
    invariants::InvariantOptions inv{
        "alias",
        invariants::AliasInvariantParameters{std::string(original_name)}};
    spdlog::info("Creating an alias invariant {}", nlohmann::json(inv).dump());
    invariants[std::string(name)] = nlohmann::json(inv);
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(OperationOptions)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, type, enabled, priority, invariants, parameters)
    if (nlohmann_json_t.priority) {
        nlohmann_json_j["priority"] = nlohmann_json_t.priority;
    }
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(OperationOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(OperationOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(
        mesh_path,
        enabled,
        parameters,
        invariants,
        type);


    if (nlohmann_json_j.contains("priority")) {
        nlohmann_json_t.priority = nlohmann_json_j["priority"];
    }

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
auto EdgeSwapOptions::get_parameters() const -> Parameters
{
    return parameters.get<Parameters>();
    //
}
void EdgeSwapOptions::set_parameters(const Parameters& p)
{
    parameters = p;
}

EdgeSwapMode EdgeSwapOptions::mode() const
{
    return get_parameters().mode;
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSwapOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeSwapOptions::Parameters)
{
    {
        std::string r;
        switch (nlohmann_json_t.mode) {
        case EdgeSwapMode::AMIPS: r = "amips"; break;
        case EdgeSwapMode::Valence: r = "valence"; break;
        default: break;
        }
        nlohmann_json_j["mode"] = r;
    }
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSwapOptions::Parameters)
{
    // WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(mode);
    if (!nlohmann_json_j.contains("mode")) {
        wmtk::logger().debug("Edge swap did not have a mode, defaulting to valence");
        nlohmann_json_t.mode = EdgeSwapMode::Valence;
    } else {
        const std::string swap_name = nlohmann_json_j["mode"].get<std::string>();
        if (swap_name == "amips") {
            nlohmann_json_t.mode = EdgeSwapMode::AMIPS;
        } else if (swap_name == "valence") {
            nlohmann_json_t.mode = EdgeSwapMode::Valence;
        } else {
            throw std::runtime_error(
                fmt::format(
                    "Expected edge_swap_mode to be one of [amips,valence], got [{}]",
                    swap_name));
        }
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

OperationOptions::~OperationOptions() = default;
OperationOptions::OperationOptions() = default;
OperationOptions::OperationOptions(const OperationOptions& o) = default;
OperationOptions::OperationOptions(OperationOptions&&) = default;
OperationOptions& OperationOptions::operator=(const OperationOptions&) = default;
OperationOptions& OperationOptions::operator=(OperationOptions&&) = default;

EdgeSplitOptions::EdgeSplitOptions()
{
    type = type_name;
}
EdgeSplitOptions::EdgeSplitOptions(const OperationOptions& o)
    : OperationOptions(o)
{
    assert(type == type_name);
}
EdgeSplitOptions::EdgeSplitOptions(const EdgeSplitOptions&) = default;
EdgeSplitOptions::EdgeSplitOptions(EdgeSplitOptions&&) = default;
EdgeSplitOptions& EdgeSplitOptions::operator=(const EdgeSplitOptions&) = default;
EdgeSplitOptions& EdgeSplitOptions::operator=(EdgeSplitOptions&&) = default;
EdgeCollapseOptions::EdgeCollapseOptions()
{
    type = type_name;
}
EdgeCollapseOptions::EdgeCollapseOptions(const OperationOptions& o)
    : OperationOptions(o)
{
    assert(type == type_name);
}
EdgeCollapseOptions::EdgeCollapseOptions(const EdgeCollapseOptions&) = default;
EdgeCollapseOptions::EdgeCollapseOptions(EdgeCollapseOptions&&) = default;
EdgeCollapseOptions& EdgeCollapseOptions::operator=(const EdgeCollapseOptions&) = default;
EdgeCollapseOptions& EdgeCollapseOptions::operator=(EdgeCollapseOptions&&) = default;

EdgeSwapOptions::EdgeSwapOptions()
{
    type = type_name;
}
EdgeSwapOptions::EdgeSwapOptions(const OperationOptions& o)
    : OperationOptions(o)
{
    assert(type == type_name);
}
EdgeSwapOptions::EdgeSwapOptions(const EdgeSwapOptions&) = default;
EdgeSwapOptions::EdgeSwapOptions(EdgeSwapOptions&&) = default;
EdgeSwapOptions& EdgeSwapOptions::operator=(const EdgeSwapOptions&) = default;
EdgeSwapOptions& EdgeSwapOptions::operator=(EdgeSwapOptions&&) = default;

VertexSmoothOptions::VertexSmoothOptions()
{
    type = type_name;
}
VertexSmoothOptions::VertexSmoothOptions(const OperationOptions& o)
    : OperationOptions(o)
{
    assert(type == type_name);
}
VertexSmoothOptions::VertexSmoothOptions(const VertexSmoothOptions&) = default;
VertexSmoothOptions::VertexSmoothOptions(VertexSmoothOptions&&) = default;
VertexSmoothOptions& VertexSmoothOptions::operator=(const VertexSmoothOptions&) = default;
VertexSmoothOptions& VertexSmoothOptions::operator=(VertexSmoothOptions&&) = default;

AttributeUpdateOptions::AttributeUpdateOptions()
{
    type = type_name;
}
AttributeUpdateOptions::AttributeUpdateOptions(const OperationOptions& o)
    : OperationOptions(o)
{
    assert(type == type_name);
}
AttributeUpdateOptions::AttributeUpdateOptions(const AttributeUpdateOptions&) = default;
AttributeUpdateOptions::AttributeUpdateOptions(AttributeUpdateOptions&&) = default;
AttributeUpdateOptions& AttributeUpdateOptions::operator=(const AttributeUpdateOptions&) = default;
AttributeUpdateOptions& AttributeUpdateOptions::operator=(AttributeUpdateOptions&&) = default;

} // namespace wmtk::components::configurator::operations
