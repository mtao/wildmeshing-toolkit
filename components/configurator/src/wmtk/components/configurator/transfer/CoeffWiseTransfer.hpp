#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategy.hpp"
#include "wmtk/components/multimesh/utils/AttributeDescription.hpp"

namespace wmtk::components::configurator::transfer {

struct CoeffWiseTransfer : public TransferStrategy
{
    MinNeighbor();
    ~MinNeighbor();
    multimesh::utils::AttributeDescription base_attribute;
    int8_t simplex_dimension;
    void to_json(nlohmann::json&) const final;
    void from_json(const nlohmann::json&) final;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MinNeighbor)
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const final;
    std::unique_ptr<TransferStrategy> clone() const final;
};

} // namespace wmtk::components::configurator::transfer
