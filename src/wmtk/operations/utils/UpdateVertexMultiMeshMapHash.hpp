#pragma once

#include <wmtk/attribute/Accessor.hpp>
// #include <wmtk/EdgeMesh.hpp>
// #include <wmtk/Mesh.hpp>
// #include <wmtk/PointMesh.hpp>
// #include <wmtk/TetMesh.hpp>
// #include <wmtk/TriMesh.hpp>

namespace wmtk {
class Mesh;
class PointMesh;
class EdgeMesh;
class TriMesh;
class TetMesh;
class Tuple;
namespace simplex {
class SimplexCollection;
}
} // namespace wmtk


namespace wmtk::operations::utils {

#if defined(WMTK_ENABLE_HASH_UPDATE) 
void update_vertex_operation_hashes(Mesh& m, const Tuple& vertex, attribute::Accessor<int64_t>& hash_accessor);

void update_vertex_operation_multimesh_map_hash(
    Mesh& m,
    const simplex::SimplexCollection& vertex_closed_star,
    attribute::Accessor<int64_t>& parent_hash_accessor);
#endif

} // namespace wmtk::operations::utils
