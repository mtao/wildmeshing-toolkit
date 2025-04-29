#pragma once

#include <array>
#include <map>
#include <memory>
#include <string_view>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;

}
} // namespace wmtk
namespace wmtk::components::multimesh {

// returns the new root mesh (the input mesh becomes a child mesh)
    std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(wmtk::Mesh& m);

} // namespace wmtk::components::multimesh
