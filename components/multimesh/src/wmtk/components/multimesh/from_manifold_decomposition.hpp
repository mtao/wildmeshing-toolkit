#pragma once

#include <array>
#include <map>
#include <memory>
#include <string_view>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;

}
} // namespace wmtk
namespace wmtk::components::multimesh {
    struct NonManifoldCascade {


        // simplices with respect to the parent mesh's vertex indices
        MatrixXl S;

        std::vector<dart::Dart> parent_map;


        NonManifoldCascade* m_parent = nullptr;
        std::unique_ptr<NonManifoldCascade> m_child = nullptr;
    };

// returns the new root mesh (the input mesh becomes a child mesh)
    NonManifoldCascade cascade_from_manifold_decomposition(wmtk::Mesh& m);

    std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(wmtk::Mesh& m);

} // namespace wmtk::components::multimesh
