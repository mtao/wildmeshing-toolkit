#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>

#include "EdgeSwapMode.hpp"
namespace wmtk {
namespace operations {
class AttributeTransferStrategyBase;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk

namespace wmtk::components::isotropic_remeshing {

struct PriorityOptions
{
    static PriorityOptions create(const nlohmann::json& js);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PriorityOptions)
};
struct InvariantOptions
{
    std::string mesh_path;
    std::string type;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
};

struct TransferStrategyOptions;

struct TransferStrategyData
{
    virtual ~TransferStrategyData();
    virtual void to_json(nlohmann::json&) const = 0;
    virtual void from_json(const nlohmann::json&) = 0;
    virtual std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,


        const TransferStrategyOptions& opts) const = 0;
    virtual std::unique_ptr<TransferStrategyData> clone() const = 0;
};

struct EdgeLengthTransferStrategyData : public TransferStrategyData
{
    EdgeLengthTransferStrategyData();
    ~EdgeLengthTransferStrategyData();
    std::string position_attribute;
    void to_json(nlohmann::json&) const final;
    void from_json(const nlohmann::json&) final;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeLengthTransferStrategyData)
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const final;
    std::unique_ptr<TransferStrategyData> clone() const final;
};

struct TransferStrategyOptions
{
    TransferStrategyOptions() = default;
    TransferStrategyOptions(const TransferStrategyOptions&);
    TransferStrategyOptions(TransferStrategyOptions&&) = default;
    TransferStrategyOptions& operator=(const TransferStrategyOptions&);
    TransferStrategyOptions& operator=(TransferStrategyOptions&&) = default;
    std::string attribute_path;
    std::string type;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(TransferStrategyOptions)
    std::unique_ptr<TransferStrategyData> data;
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc) const;
};

struct MinEdgeLengthInavariant
{
    std::string type;
    std::string mesh_path;
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
