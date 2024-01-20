#pragma once
#include <wmtk/Types.hpp>
#include <memory>

namespace wmtk {
    class Mesh;
    class TriMesh;
    class TetMesh;
}
namespace wmtk::utils {

    /// When passed a mesh with its top dimension adjacency map
    void topology_from_boundaries(Mesh& m);
    std::shared_ptr<TriMesh> trimesh_topology_from_boundaries(const Eigen::Ref<RowVectors3l> FF);
    std::shared_ptr<TetMesh> tetmesh_topology_from_boundaries(const Eigen::Ref<RowVectors3l> TT);
}
