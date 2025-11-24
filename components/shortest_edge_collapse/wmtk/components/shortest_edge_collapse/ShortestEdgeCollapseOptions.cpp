#include "ShortestEdgeCollapseOptions.hpp"
#include <spdlog/spdlog.h>

namespace wmtk::components::shortest_edge_collapse {

ShortestEdgeCollapseOptions::ShortestEdgeCollapseOptions(
    const multimesh::utils::AttributeDescription& position_handle,
    const double length_rel,
    std::optional<bool> lock_boundary,
    std::optional<double> envelope_size,
    bool check_inversion,
    const std::vector<multimesh::utils::AttributeDescription>& pass_through)
{
    this->position_handle = position_handle;
    this->length_rel = length_rel;
    if (lock_boundary) {
        lock_boundary = lock_boundary.value();
    }
    this->envelope_size = envelope_size;

    update_edge_length_transfer();


    pass_through_attributes = pass_through;
    spdlog::warn("Default stuff: {}", position_handle, nlohmann::json(transfers).dump(2));
}

void ShortestEdgeCollapseOptions::update_edge_length_transfer()
{
    transfers.emplace(
        "edge_length",
        configurator::transfer::TransferStrategyOptions{
            .attribute = multimesh::utils::AttributeDescription(
                "edge_length",
                1,
                attribute::AttributeType::Double,
                1),
            .type = "edge_length",
            .parameters = configurator::transfer::SingleAttributeTransferStrategyParameters{
                .attribute = position_handle}});
}
} // namespace wmtk::components::shortest_edge_collapse
