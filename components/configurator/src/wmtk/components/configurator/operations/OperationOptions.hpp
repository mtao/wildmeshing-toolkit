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


struct OperationOptions
{
    OperationOptions();
    OperationOptions(const OperationOptions&);
    OperationOptions(OperationOptions&&);
    OperationOptions& operator=(const OperationOptions&);
    OperationOptions& operator=(OperationOptions&&);
    ~OperationOptions();
    // std::string name;
    std::string type;
    std::string mesh_path;
    bool enabled = true;
    PriorityOptions priority;
    std::map<std::string, invariants::InvariantOptions> invariants;
    // adds a new alias invariant assuming the name in the operation is the same as the
    // configurator's name
    void add_alias_invariant(std::string_view name);
    // adds a new alias invariant for "original_name" but giving the name "name"
    void add_alias_invariant(std::string_view name, std::string_view original_name);

    nlohmann::json parameters;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(OperationOptions)
};

struct EdgeSplitOptions : public OperationOptions
{
    EdgeSplitOptions();
    EdgeSplitOptions(const OperationOptions& o);
    EdgeSplitOptions(const EdgeSplitOptions&);
    EdgeSplitOptions(EdgeSplitOptions&&);
    EdgeSplitOptions& operator=(const EdgeSplitOptions&);
    EdgeSplitOptions& operator=(EdgeSplitOptions&&);
    constexpr static std::string type_name = "edge_split";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSplitOptions)
};
struct EdgeCollapseOptions : public OperationOptions
{
    EdgeCollapseOptions();
    EdgeCollapseOptions(const OperationOptions& o);
    EdgeCollapseOptions(const EdgeCollapseOptions&);
    EdgeCollapseOptions(EdgeCollapseOptions&&);
    EdgeCollapseOptions& operator=(const EdgeCollapseOptions&);
    EdgeCollapseOptions& operator=(EdgeCollapseOptions&&);
    constexpr static std::string type_name = "edge_collapse";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeCollapseOptions)
};
struct EdgeSwapOptions : public OperationOptions
{
    struct Parameters
    {
        EdgeSwapMode mode;
        std::map<std::string, invariants::InvariantOptions> split_invariants;
        std::map<std::string, invariants::InvariantOptions> collapse_invariants;

        WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Parameters)
    };
    Parameters get_parameters() const;
    void set_parameters(const Parameters& p);
    EdgeSwapOptions();
    EdgeSwapOptions(const OperationOptions& o);
    EdgeSwapOptions(const EdgeSwapOptions&);
    EdgeSwapOptions(EdgeSwapOptions&&);
    EdgeSwapOptions& operator=(const EdgeSwapOptions&);
    EdgeSwapOptions& operator=(EdgeSwapOptions&&);
    // by default the mode is set to Valence;
    EdgeSwapMode mode() const;
    void set_mode(EdgeSwapMode);
    constexpr static std::string type_name = "edge_swap";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSwapOptions)
};
struct VertexSmoothOptions : public OperationOptions
{
    VertexSmoothOptions();
    VertexSmoothOptions(const OperationOptions& o);
    VertexSmoothOptions(const VertexSmoothOptions&);
    VertexSmoothOptions(VertexSmoothOptions&&);
    VertexSmoothOptions& operator=(const VertexSmoothOptions&);
    VertexSmoothOptions& operator=(VertexSmoothOptions&&);
    constexpr static std::string type_name = "vertex_smooth";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(VertexSmoothOptions)
};
struct AttributeUpdateOptions : public OperationOptions
{
    struct Parameters
    {
        bool project = false;
        WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Parameters)
    };
    AttributeUpdateOptions();
    AttributeUpdateOptions(const OperationOptions& o);
    AttributeUpdateOptions(const AttributeUpdateOptions&);
    AttributeUpdateOptions(AttributeUpdateOptions&&);
    AttributeUpdateOptions& operator=(const AttributeUpdateOptions&);
    AttributeUpdateOptions& operator=(AttributeUpdateOptions&&);
    constexpr static std::string type_name = "attr_update";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeUpdateOptions)
};

struct Pass
{
    std::string mesh_path;
    int64_t iterations = 10;
    std::vector<std::string> operations;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Pass)
};


} // namespace wmtk::components::configurator::operations
