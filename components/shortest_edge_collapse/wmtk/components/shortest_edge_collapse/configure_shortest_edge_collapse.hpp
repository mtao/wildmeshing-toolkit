#pragma once

#include <optional>
#include <vector>
#include <wmtk/components/configurator/PassConfiguration.hpp>
#include "ShortestEdgeCollapseOptions.hpp"

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;

}
} // namespace wmtk

namespace wmtk::components::multimesh {
class MeshCollection;
}

namespace wmtk::components::shortest_edge_collapse {

configurator::PassConfiguration configure_shortest_edge_collapse(
    const ShortestEdgeCollapseOptions& options);


} // namespace wmtk::components::shortest_edge_collapse
