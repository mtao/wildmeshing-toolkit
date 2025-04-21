#pragma once

#include <memory>
#include <string_view>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;

}
namespace utils::internal {
class IndexSimplexMapper;
}
} // namespace wmtk
namespace wmtk::components::multimesh {
class MeshCollection;
std::vector<std::array<Tuple, 2>> from_vertex_simplices_map(
    Mesh& parent,
    const wmtk::utils::internal::IndexSimplexMapper& indexer,
    const MatrixXl& l);

std::vector<std::array<Tuple, 2>> from_vertex_simplices_map(Mesh& parent, const MatrixXl& l);
std::shared_ptr<Mesh> from_vertex_simplices(Mesh& parent, const MatrixXl& l);

} // namespace wmtk::components::multimesh
