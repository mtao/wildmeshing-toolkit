#pragma once

#include <memory>
#include <string_view>
#include <wmtk/Types.hpp>

namespace wmtk {
class Mesh;
namespace attribute {
    class MeshAttributeHandle;

}
} // namespace wmtk
namespace wmtk::components::multimesh {

std::shared_ptr<Mesh> from_vertex_simplices(Mesh& parent, const MatrixXl& l, const std::string_view& name);

} // namespace wmtk::components::multimesh
