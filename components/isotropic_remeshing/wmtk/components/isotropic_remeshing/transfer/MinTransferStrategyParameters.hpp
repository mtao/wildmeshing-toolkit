#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategyParameters.hpp"

namespace wmtk::components::isotropic_remeshing::transfer {

struct MinTransferStrategyParameters : public TransferStrategyParameters
{
    MinTransferStrategyParameters();
    ~MinTransferStrategyParameters();
    std::string base_attribute_path;
    int8_t simplex_dimension;
    void to_json(nlohmann::json&) const final;
    void from_json(const nlohmann::json&) final;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MinTransferStrategyParameters)
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const final;
    std::unique_ptr<TransferStrategyParameters> clone() const final;
};

} // namespace wmtk::components::isotropic_remeshing::transfer
