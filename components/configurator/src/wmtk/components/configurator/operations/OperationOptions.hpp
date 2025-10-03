#pragma once
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>

#include "../invariants/InvariantOptions.hpp"
#include "../transfer/TransferStrategyOptions.hpp"
#include "EdgeSwapMode.hpp"
#include "PriorityOptions.hpp"
#include "wmtk/components/multimesh/utils/AttributeDescription.hpp"

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
    OperationOptions(std::string_view type = {});
    OperationOptions(const OperationOptions&);
    OperationOptions(OperationOptions&&);
    OperationOptions& operator=(const OperationOptions&);
    OperationOptions& operator=(OperationOptions&&);
    ~OperationOptions();
    // std::string name;
    std::string type;
    PriorityOptions priority;
    std::map<std::string, invariants::InvariantOptions> invariants;
    std::map<std::string, transfer::TransferStrategyOptions> transfers;
    // adds a new alias invariant assuming the name in the operation is the same as the
    // configurator's name
    void add_alias_invariant(std::string_view name);
    // adds a new alias invariant for "original_name" but giving the name "name"
    void add_alias_invariant(std::string_view name, std::string_view original_name);

    nlohmann::json parameters;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(OperationOptions)
};

struct MeshOperationParameters
{
    MeshOperationParameters(std::string_view mp = {})
        : mesh_path(std::string(mp))
    {}
    MeshOperationParameters(const MeshOperationParameters&) = default;
    MeshOperationParameters(MeshOperationParameters&&) = default;
    MeshOperationParameters& operator=(const MeshOperationParameters&) = default;
    MeshOperationParameters& operator=(MeshOperationParameters&&) = default;
    std::string mesh_path;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshOperationParameters)
};

struct EdgeSplitOptions : public OperationOptions
{
    MeshOperationParameters get_parameters() const;
    void set_parameters(const MeshOperationParameters& p);
    EdgeSplitOptions(std::string_view mesh_path = "");
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
    MeshOperationParameters get_parameters() const;
    void set_parameters(const MeshOperationParameters& p);
    EdgeCollapseOptions(std::string_view mesh_path = "");
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
    struct Parameters : public MeshOperationParameters
    {
        using MeshOperationParameters::MeshOperationParameters;
        //EdgeSwapMode mode = EdgeSwapMode::Valence;
        std::map<std::string, invariants::InvariantOptions> split_invariants;
        std::map<std::string, invariants::InvariantOptions> collapse_invariants;

        WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Parameters)
    };
    Parameters get_parameters() const;
    void set_parameters(const Parameters& p);
    EdgeSwapOptions(std::string_view mesh_path = "");
    EdgeSwapOptions(const OperationOptions& o);
    EdgeSwapOptions(const EdgeSwapOptions&);
    EdgeSwapOptions(EdgeSwapOptions&&);
    EdgeSwapOptions& operator=(const EdgeSwapOptions&);
    EdgeSwapOptions& operator=(EdgeSwapOptions&&);
    // by default the mode is set to Valence;
    //EdgeSwapMode mode() const;
    //void set_mode(EdgeSwapMode);
    constexpr static std::string type_name = "edge_swap";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeSwapOptions)
};
struct AttributeUpdateOptions : public OperationOptions
{
    struct Parameters
    {
        multimesh::utils::AttributeDescription attribute;
        std::string function;
        // if this value is set then a mesh to project to will be enabled
        std::string projection_attribute;
        WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Parameters)
    };

    Parameters get_parameters() const;
    void set_parameters(const Parameters& p);
    AttributeUpdateOptions(const multimesh::utils::AttributeDescription& attr = {}, std::string_view function_name = {});
    AttributeUpdateOptions(const OperationOptions& o);
    AttributeUpdateOptions(const AttributeUpdateOptions&);
    AttributeUpdateOptions(AttributeUpdateOptions&&);
    AttributeUpdateOptions& operator=(const AttributeUpdateOptions&);
    AttributeUpdateOptions& operator=(AttributeUpdateOptions&&);
    constexpr static std::string type_name = "attr_update";
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeUpdateOptions)
};



} // namespace wmtk::components::configurator::operations
