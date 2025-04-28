#pragma once

#include <memory>
#include <string_view>
#include <vector>

namespace wmtk {
class Mesh;
namespace attribute {
    class MeshAttributeHandle;

}
} // namespace wmtk
namespace wmtk::components::multimesh {

// returns the new root mesh (the input mesh becomes a child mesh)
    std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(const wmtk::Mesh& m);

} // namespace wmtk::components::multimesh
