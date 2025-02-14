#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategy.hpp"

namespace wmtk::components::mesh_info::transfer {

struct MeanNeighbor : public TransferStrategy
{
    MeanNeighbor();
    ~MeanNeighbor() override;
    std::string base_attribute_path;
    int8_t simplex_dimension;
    void to_json(nlohmann::json&) const final;
    void from_json(const nlohmann::json&) final;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeanNeighbor)
    auto create(
        wmtk::components::multimesh::MeshCollection& mc,
        const TransferStrategyOptions& opts) const
        -> std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> final;
    auto clone() const -> std::unique_ptr<TransferStrategy> final;
};

} // namespace wmtk::components::mesh_info::transfer
