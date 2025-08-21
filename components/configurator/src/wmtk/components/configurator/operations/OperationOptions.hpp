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
    OperationOptions() = default;
    OperationOptions(const OperationOptions&) = default;
    OperationOptions(OperationOptions&&) = default;
    OperationOptions& operator=(const OperationOptions&) = default;
    OperationOptions& operator=(OperationOptions&&) = default;
    ~OperationOptions() = default;
    // std::string name;
    std::string type;
    std::string mesh_path;
    bool enabled = true;
    PriorityOptions priority;
    std::map<std::string, invariants::InvariantOptions> invariants;
    nlohmann::json parameters;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(OperationOptions)
};

struct EdgeSplitOptions : public OperationOptions
{
    EdgeSplitOptions() = default;
    EdgeSplitOptions(const OperationOptions& o)
        : OperationOptions(o)
    {}
    EdgeSplitOptions(const EdgeSplitOptions&) = default;
    EdgeSplitOptions(EdgeSplitOptions&&) = default;
    EdgeSplitOptions& operator=(const EdgeSplitOptions&) = default;
    EdgeSplitOptions& operator=(EdgeSplitOptions&&) = default;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSplitOptions)
};
struct EdgeCollapseOptions : public OperationOptions
{
    EdgeCollapseOptions() = default;
    EdgeCollapseOptions(const OperationOptions& o)
        : OperationOptions(o)
    {}
    EdgeCollapseOptions(const EdgeCollapseOptions&) = default;
    EdgeCollapseOptions(EdgeCollapseOptions&&) = default;
    EdgeCollapseOptions& operator=(const EdgeCollapseOptions&) = default;
    EdgeCollapseOptions& operator=(EdgeCollapseOptions&&) = default;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeCollapseOptions)
};
struct EdgeSwapOptions : public OperationOptions
{
    EdgeSwapOptions() = default;
    EdgeSwapOptions(const OperationOptions& o)
        : OperationOptions(o)
    {}
    EdgeSwapOptions(const EdgeSwapOptions&) = default;
    EdgeSwapOptions(EdgeSwapOptions&&) = default;
    EdgeSwapOptions& operator=(const EdgeSwapOptions&) = default;
    EdgeSwapOptions& operator=(EdgeSwapOptions&&) = default;
    // by default the mode is set to Valence;
    EdgeSwapMode mode() const;
    void set_mode(EdgeSwapMode);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSwapOptions)
};
struct VertexSmoothOptions : public OperationOptions
{
    VertexSmoothOptions() = default;
    VertexSmoothOptions(const OperationOptions& o)
        : OperationOptions(o)
    {}
    VertexSmoothOptions(const VertexSmoothOptions&) = default;
    VertexSmoothOptions(VertexSmoothOptions&&) = default;
    VertexSmoothOptions& operator=(const VertexSmoothOptions&) = default;
    VertexSmoothOptions& operator=(VertexSmoothOptions&&) = default;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(VertexSmoothOptions)
};
struct AttributeUpdateOptions : public OperationOptions
{
    AttributeUpdateOptions() = default;
    AttributeUpdateOptions(const OperationOptions& o)
        : OperationOptions(o)
    {}
    AttributeUpdateOptions(const AttributeUpdateOptions&) = default;
    AttributeUpdateOptions(AttributeUpdateOptions&&) = default;
    AttributeUpdateOptions& operator=(const AttributeUpdateOptions&) = default;
    AttributeUpdateOptions& operator=(AttributeUpdateOptions&&) = default;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeUpdateOptions)
};
struct ProjectedAttributeUpdateOptions : public AttributeUpdateOptions
{
    ProjectedAttributeUpdateOptions() = default;
    ProjectedAttributeUpdateOptions(const OperationOptions& o)
        : AttributeUpdateOptions(o)
    {}
    ProjectedAttributeUpdateOptions(const ProjectedAttributeUpdateOptions&) = default;
    ProjectedAttributeUpdateOptions(ProjectedAttributeUpdateOptions&&) = default;
    ProjectedAttributeUpdateOptions& operator=(const ProjectedAttributeUpdateOptions&) = default;
    ProjectedAttributeUpdateOptions& operator=(ProjectedAttributeUpdateOptions&&) = default;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(ProjectedAttributeUpdateOptions)
};

struct Pass
{
    std::string mesh_path;
    int64_t iterations = 10;
    std::vector<std::string> operations;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Pass)
};


} // namespace wmtk::components::configurator::operations
