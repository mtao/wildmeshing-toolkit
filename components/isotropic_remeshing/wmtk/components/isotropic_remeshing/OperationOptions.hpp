#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>

#include "EdgeSwapMode.hpp"

namespace wmtk::components::isotropic_remeshing {

struct PriorityOptions
{
    std::string type;
    std::string attribute_path; // TODO move this into a child 
    static PriorityOptions create(const nlohmann::json& js);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PriorityOptions)
};


struct InvariantParameters {};

struct InvariantOptions
{
    std::string type;
    std::unique_ptr<InvariantParameters> parameters;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
};


struct AttributeInvariantParameters: public InvariantParameters
{
    std::string attribute_path;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
};

struct OperationOptions
{
    std::string mesh_path;
    bool enabled = true;
    std::shared_ptr<PriorityOptions> priority;
    std::vector<std::shared_ptr<InvariantOptions>> invariants;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(OperationOptions)
};

struct EdgeSplitOptions : public OperationOptions
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSplitOptions)
};
struct EdgeCollapseOptions : public OperationOptions
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeCollapseOptions)
};
struct EdgeSwapOptions : public OperationOptions
{
    EdgeSwapMode mode = EdgeSwapMode::Skip;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSwapOptions)
};
struct VertexSmoothOptions : public OperationOptions
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(VertexSmoothOptions)
};
} // namespace wmtk::components::isotropic_remeshing
