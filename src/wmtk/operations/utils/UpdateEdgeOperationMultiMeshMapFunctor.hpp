#pragma once
#include <Eigen/Core>
#include <memory>
#include <vector>
#include <wmtk/multimesh/MultiMeshManager.hpp>

namespace wmtk {
class Mesh;
class PointMesh;
class EdgeMesh;
class TriMesh;
class TetMesh;
class Tuple;
namespace simplex {
class Simplex;
}
namespace attribute {
template <typename T>
class CachingAttribute;
template <typename T, typename MeshType, typename AttributeType, int Dim>
class Accessor;
} // namespace attribute

namespace operations {
class EdgeOperationData;
namespace edge_mesh {
class EdgeOperationData;
}
namespace tri_mesh {
class EdgeOperationData;
}
namespace tet_mesh {
class EdgeOperationData;
}
} // namespace operations

namespace operations::utils {

class UpdateEdgeOperationMultiMeshMapFunctor
{
public:
    // edge -> edge
    void operator()(
        EdgeMesh&,
        const simplex::Simplex&,
        const edge_mesh::EdgeOperationData& parent_tmoe,
        EdgeMesh&,
        const simplex::Simplex&,
        const edge_mesh::EdgeOperationData&) const;

    // tri -> edge
    void operator()(
        TriMesh&,
        const simplex::Simplex&,
        const tri_mesh::EdgeOperationData&,
        EdgeMesh&,
        const simplex::Simplex&,
        const edge_mesh::EdgeOperationData&) const;
    // tri -> tri
    void operator()(
        TriMesh&,
        const simplex::Simplex&,
        const tri_mesh::EdgeOperationData&,
        TriMesh&,
        const simplex::Simplex&,
        const tri_mesh::EdgeOperationData&) const;

    // tet -> edge
    void operator()(
        TetMesh&,
        const simplex::Simplex&,
        const tet_mesh::EdgeOperationData&,
        EdgeMesh&,
        const simplex::Simplex&,
        const edge_mesh::EdgeOperationData&) const;
    // tet -> tri
    void operator()(
        TetMesh&,
        const simplex::Simplex&,
        const tet_mesh::EdgeOperationData&,
        TriMesh&,
        const simplex::Simplex&,
        const tri_mesh::EdgeOperationData&) const;
    // tet -> tet
    void operator()(
        TetMesh&,
        const simplex::Simplex&,
        const tet_mesh::EdgeOperationData&,
        TetMesh&,
        const simplex::Simplex&,
        const tet_mesh::EdgeOperationData&) const;

    // edge
    void operator()(EdgeMesh&, const simplex::Simplex&, const edge_mesh::EdgeOperationData&);

    // tri
    void operator()(TriMesh&, const simplex::Simplex&, const tri_mesh::EdgeOperationData&);

    // tet
    void operator()(TetMesh&, const simplex::Simplex&, const tet_mesh::EdgeOperationData&);

private:
    /*
    template <typename T>
    void update_maps(MeshType&, const simplex::Simplex&, const EdgeOperationData&);
    */

private:
    int64_t parent_global_cid(
        const multimesh::MultiMeshManager::AccessorType& parent_to_child,
        int64_t parent_gid) const;
    int64_t child_global_cid(
        const multimesh::MultiMeshManager::AccessorType& parent_to_child,
        int64_t parent_gid) const;
    // for tet
    int64_t parent_local_fid(
        const multimesh::MultiMeshManager::AccessorType& parent_to_child,
        int64_t parent_gid) const;
    void update_all_hashes(
        Mesh& m,
        const std::vector<std::vector<std::tuple<int64_t, std::vector<Tuple>>>>&
            simplices_to_update,
        const std::vector<std::tuple<int64_t, std::array<int64_t, 2>>>& split_cell_maps = {}) const;
    void update_all_maps(Mesh& m, const EdgeOperationData& eod) const;

    void update_ear_replacement(EdgeMesh& m, const edge_mesh::EdgeOperationData& emoe) const;

    void update_ear_replacement(TriMesh& m, const tri_mesh::EdgeOperationData& fmoe) const;


    void update_ear_replacement(TetMesh& m, const tet_mesh::EdgeOperationData& tmoe) const;
};
} // namespace operations::utils
} // namespace wmtk
