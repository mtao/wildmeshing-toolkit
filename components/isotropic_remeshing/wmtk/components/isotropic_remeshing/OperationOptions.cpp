#include "OperationOptions.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/simplex/Simplex.hpp>

namespace wmtk::components::isotropic_remeshing {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PriorityOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type, attribute_path, minimize)
    //
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PriorityOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(PriorityOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(type, attribute_path, minimize);
}

void PriorityOptions::assign_to(
    const wmtk::components::multimesh::MeshCollection& mc,
    wmtk::operations::Operation& op) const
{
    if (type == "random") {
        op.use_random_priority() = false;
        op.set_priority(nullptr);
    } else if (type == "random") {
        op.use_random_priority() = true;
        op.set_priority(nullptr);
    } else if (type == "attribute") {
        op.use_random_priority() = false;
        auto ap = attribute_path;
        if (!ap.empty()) {
            auto priority_attribute = wmtk::components::multimesh::utils::get_attribute(
                mc,
                wmtk::components::multimesh::utils::AttributeDescription{ap});
            if(minimize) {
            auto priority_func = [priority_attribute](const simplex::Simplex& s) -> double {
                auto acc =
                    priority_attribute.mesh().create_const_accessor<double>(priority_attribute);
                return acc.const_scalar_attribute(s);
            };
            op.set_priority(priority_func);
            } else {
            auto priority_func = [priority_attribute](const simplex::Simplex& s) -> double {
                auto acc =
                    priority_attribute.mesh().create_const_accessor<double>(priority_attribute);
                return -acc.const_scalar_attribute(s);
            };
            op.set_priority(priority_func);
            }
        }
    } else {
        assert(false); // tried to assign an invalid priority
    }
}


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(InvariantOptions)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type)

    auto ap = static_cast<const AttributeInvariantParameters*>(nlohmann_json_t.parameters.get())
                  ->attribute_path;
    nlohmann_json_j["attribute_path"] = ap;
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(InvariantOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(type)
    auto p = std::make_unique<AttributeInvariantParameters>();
    p->attribute_path = nlohmann_json_j["attribute_path"];
    nlohmann_json_t.parameters = std::move(p);
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(OperationOptions)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, enabled)
    if (const auto& p = nlohmann_json_t.priority; p) {
        nlohmann_json_j["priority"] = *p;
    }
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(OperationOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(OperationOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, enabled);

    if (nlohmann_json_j.contains("priority")) {
        auto p = std::make_shared<PriorityOptions>();
        *p = nlohmann_json_j["priority"];
        nlohmann_json_t.priority = std::move(p);
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


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(Pass)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path,iterations)
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(Pass)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(mesh_path, iterations)
}

} // namespace wmtk::components::isotropic_remeshing
