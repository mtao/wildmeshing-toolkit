#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategyParameters.hpp"

namespace wmtk::components::mesh_info::transfer {

struct EdgeLengthTransferStrategyParameters : public TransferStrategyParameters
{
    EdgeLengthTransferStrategyParameters();
    ~EdgeLengthTransferStrategyParameters();
    std::string position_attribute;
    void to_json(nlohmann::json&) const final;
    void from_json(const nlohmann::json&) final;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EdgeLengthTransferStrategyParameters)
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const final;
    std::unique_ptr<TransferStrategyParameters> clone() const final;
};

} // namespace wmtk::components::mesh_info::transfer
