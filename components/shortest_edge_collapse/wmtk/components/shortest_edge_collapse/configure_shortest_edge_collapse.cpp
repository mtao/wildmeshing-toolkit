#include "configure_shortest_edge_collapse.hpp"
#include <fmt/format.h>
#include <wmtk/components/configurator/invariants/InvariantParameters_extra.hpp>

namespace wmtk::components::shortest_edge_collapse {

configurator::PassConfiguration configure_shortest_edge_collapse(
    const ShortestEdgeCollapseOptions& options)
{
    configurator::PassConfiguration pass_options = options;

    configurator::operations::EdgeCollapseOptions collapse_options;
    if (pass_options.operations.contains("edge_collapse")) {
        collapse_options = pass_options.operations["edge_collapse"];
    }


    std::vector<components::multimesh::utils::AttributeDescription>
        inversion_attribute_descriptions;
    if (options.check_inversions) {
        for (const auto& ph : options.other_position_handles) {
            inversion_attribute_descriptions.emplace_back(ph);
        }
    }

    {
        configurator::transfer::TransferStrategyOptions opts{
            .attribute = multimesh::utils::AttributeDescription(
                "visited_edge",
                1,
                attribute::AttributeType::Char,
                1),
            .default_value = 1,
            .type = "update_flag",
            .parameters = configurator::transfer::SingleAttributeTransferStrategyParameters{
                .attribute = options.position_handle}};
        pass_options.transfers.emplace("visited_edge", opts);
    }


    multimesh::utils::AttributeDescription edge_length_attr(

        fmt::format("{}/{}", options.position_handle.mesh_path(), "edge_length"),
        1,
        attribute::AttributeType::Double,
        1);


    {
        configurator::transfer::TransferStrategyOptions opts{
            .attribute = edge_length_attr,
            .type = "edge_length",
            .parameters = configurator::transfer::SingleAttributeTransferStrategyParameters{
                .attribute = options.position_handle}};
        pass_options.transfers.emplace("edge_length", opts);
    }
    //{
    //    configurator::invariants::ScalarValueInvariantParameters<char>
    //}
    {
        configurator::invariants::ThresholdInvariantParameters inv;
        inv.attribute = edge_length_attr;
        inv.type = configurator::invariants::ThresholdInvariantParameters::ThresholdType::
            BoundingBoxDiagonalRelative;
        configurator::invariants::BoundingBoxDiagonalThresholdParameters bb;
        bb.attribute = options.position_handle;
        bb.ratio = .8 * options.length_rel;
        inv.parameters = bb;

        configurator::invariants::InvariantOptions{"todo_less_than", inv};
        // configurator::invariants::
    }

    return pass_options;
}
