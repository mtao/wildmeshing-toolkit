#pragma once
#include <nlohmann/json_fwd.hpp>
#include "TransferStrategyOptions.hpp"
namespace wmtk {
namespace operations {
class AttributeTransferStrategyBase;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::mesh_info::transfer {
struct TransferStrategyOptions;
struct TransferStrategy
{
    virtual ~TransferStrategy();
    // virtual std::string name() const = 0;
    virtual void to_json(nlohmann::json&) const = 0;
    virtual void from_json(const nlohmann::json&) = 0;
    virtual std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const = 0;


    virtual std::unique_ptr<TransferStrategy> clone() const = 0;
};

} // namespace wmtk::components::mesh_info::transfer

