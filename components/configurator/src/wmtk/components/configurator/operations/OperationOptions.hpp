#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>

#include "../invariants/InvariantOptions.hpp"
#include "EdgeSwapMode.hpp"
#include "PriorityOptions.hpp"

namespace wmtk {
namespace operations {
class Operation;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::configurator::operations {

class PriorityOptions;


struct OperationOptions
{
    // OperationOptions();
    //~OperationOptions();
    // std::string name;
    std::string type;
    std::string mesh_path;
    bool enabled = true;
    PriorityOptions priority;
    std::map<std::string,invariants::InvariantOptions> invariants;
    nlohmann::json parameters;

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
    // by default the mode is set to Valence;
    EdgeSwapMode mode() const;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSwapOptions)
};
struct VertexSmoothOptions : public OperationOptions
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(VertexSmoothOptions)
};
struct AttributeUpdateOperation : public OperationOptions
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeUpdateOperation)
};
struct ProjectedAttributeUpdateOperation : public AttributeUpdateOperation
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(ProjectedAttributeUpdateOperation)
};

struct Pass
{
    std::string mesh_path;
    int64_t iterations = 10;
    std::vector<std::string> operations;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Pass)
};


} // namespace wmtk::components::configurator::operations
