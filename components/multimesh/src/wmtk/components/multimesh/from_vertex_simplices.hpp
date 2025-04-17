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
} // namespace wmtk
namespace wmtk::components::multimesh {

std::vector<std::pair<Tuple, Tuple>>
from_vertex_simplices_map(Mesh& parent, const MatrixXl& l, const std::string_view& name);
std::shared_ptr<Mesh>
from_vertex_simplices(Mesh& parent, const MatrixXl& l, const std::string_view& name);

} // namespace wmtk::components::multimesh
