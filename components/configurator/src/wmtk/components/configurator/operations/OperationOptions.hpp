#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>

#include "EdgeSwapMode.hpp"

namespace wmtk {
namespace operations {
class Operation;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::isotropic_remeshing {

struct PriorityOptions
{
    std::string type;
    std::string attribute_path; // TODO move this into a child
    bool minimize = false; // if true prioritizes teh largest priority operations first
    static PriorityOptions create(const nlohmann::json& js);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PriorityOptions)

    void assign_to(
        const wmtk::components::multimesh::MeshCollection& mc,
        wmtk::operations::Operation&) const;
};


struct InvariantParameters
{
    InvariantParameters();
    virtual ~InvariantParameters();
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantParameters)
    virtual void to_json(nlohmann::json& j) const = 0;
};

struct InvariantOptions
{
    std::string type;
    std::unique_ptr<InvariantParameters> parameters;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
};


struct AttributeInvariantParameters : public InvariantParameters
{
    std::string attribute_path;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantParameters)
    void to_json(nlohmann::json& j) const override;
};
struct EnvelopeInvariantParameters : public AttributeInvariantParameters
{
    double size;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantParameters)
    void to_json(nlohmann::json& j) const override;
};

struct InvariantCollectionParameters : public InvariantParameters
{
    std::vector<InvariantOptions> invariants;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantCollectionParameters)
    void to_json(nlohmann::json& j) const override;
};

// special invariant for referring to invariants from a cache
struct AliasInvariantParameters : public InvariantParameters
{
    std::string invariant_name;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AliasInvariantParameters)
    void to_json(nlohmann::json& j) const override;
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

struct Pass
{
    std::string mesh_path;
    int64_t iterations = 10;
    std::vector<std::string> operations;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Pass)
};


} // namespace wmtk::components::isotropic_remeshing
