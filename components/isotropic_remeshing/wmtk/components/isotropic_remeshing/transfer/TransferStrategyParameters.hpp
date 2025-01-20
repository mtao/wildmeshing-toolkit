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
namespace wmtk::components::isotropic_remeshing::transfer {
struct TransferStrategyOptions;
struct TransferStrategyParameters
{
    virtual ~TransferStrategyParameters();
    virtual void to_json(nlohmann::json&) const = 0;
    virtual void from_json(const nlohmann::json&) = 0;
    virtual std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const = 0;

    virtual std::unique_ptr<TransferStrategyParameters> clone() const = 0;
};

} // namespace wmtk::components::isotropic_remeshing::transfer

