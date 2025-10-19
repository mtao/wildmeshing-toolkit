#pragma once

#include <optional>
#include <vector>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include "wmtk/components/configurator/PassConfiguration.hpp"

namespace wmtk::components::shortest_edge_collapse {

struct ShortestEdgeCollapseOptions : public wmtk::components::configurator::PassConfiguration
{
    ShortestEdgeCollapseOptions() = default;
    ShortestEdgeCollapseOptions(const ShortestEdgeCollapseOptions&) = default;
    ShortestEdgeCollapseOptions(ShortestEdgeCollapseOptions&&) = default;
    auto operator=(const ShortestEdgeCollapseOptions&) -> ShortestEdgeCollapseOptions& = default;
    auto operator=(ShortestEdgeCollapseOptions&&) -> ShortestEdgeCollapseOptions& = default;
    ShortestEdgeCollapseOptions(
        const multimesh::utils::AttributeDescription& position_handle,
        const double length_rel,
        std::optional<bool> lock_boundary = {},
        std::optional<double> envelope_size = {},
        bool check_inversion = false,
        const std::vector<multimesh::utils::AttributeDescription>& pass_through = {});


    /**
     * vertex positions (double)
     */
    components::multimesh::utils::AttributeDescription position_handle;
    /**
     * If this mesh is part of a multimesh, specify the vertex positions of all other meshes here,
     * if they have any.
     */
    std::vector<components::multimesh::utils::AttributeDescription> other_position_handles;
    /**
     * The desired edge length relative to the AABB.
     */
    double length_rel;
    /**
     * Are boundary vertices allowed to be collapsed?
     */
    bool lock_boundary = false;
    /**
     * The envelope size relative to the AABB.
     */
    std::optional<double> envelope_size;
    /**
     * If this attribute is specified, it is used to check for inversions. The mesh must be of top
     * dimension, e.g., a TriMesh in 2D or a TetMesh in 3D.
     */
    bool check_inversions = false;
    /**
     * Any other attribute goes here. They are handled with the default attribute behavior.
     */
    std::vector<components::multimesh::utils::AttributeDescription> pass_through_attributes;
};

} // namespace wmtk::components::shortest_edge_collapse
