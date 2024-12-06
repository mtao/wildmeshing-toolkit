#pragma once
#include <cassert>
#include <wmtk/Tuple.hpp>
#include "autogenerated_tables.hpp"
#if !defined(_NDEBUG)
#include "is_ccw.hpp"
#endif

namespace wmtk::autogen::tet_mesh {
/*
namespace {
template <size_t Dim>
inline Tuple get_tuple_from_simplex_local_id_T(int8_t local_id, int64_t global_id)
{
const auto& arr = autogen::tet_mesh::auto_3d_table_complete_edge[local_id];
assert(arr[Dim] == local_id);
const auto& [lvid, leid, lfid] = arr;


if (lvid < 0 || leid < 0 || lfid < 0) {
    throw std::runtime_error("tuple_from_id failed");
}
Tuple tuple = Tuple(lvid, leid, lfid, global_id);
// accessor as parameter
assert(is_ccw(tuple)); // is_ccw also checks for validity
return tuple;
}
} // namespace
*/

inline Tuple get_tuple_from_simplex_local_vertex_id(int8_t local_id, int64_t global_id)
{
    const auto& arr = autogen::tet_mesh::auto_3d_table_complete_vertex[local_id];
    const auto& [lvid, leid, lfid] = arr;
    assert(lvid == local_id);


    if (lvid < 0 || leid < 0 || lfid < 0) {
        throw std::runtime_error("tuple_from_vertex_id failed");
    }
    Tuple tuple = Tuple(lvid, leid, lfid, global_id);
    // accessor as parameter
    assert(is_ccw(tuple)); // is_ccw also checks for validity
    return tuple;
}
inline Tuple get_tuple_from_simplex_local_edge_id(int8_t local_id, int64_t global_id)
{
    const auto& arr = autogen::tet_mesh::auto_3d_table_complete_edge[local_id];
    const auto& [lvid, leid, lfid] = arr;
    assert(leid == local_id);


    if (lvid < 0 || leid < 0 || lfid < 0) {
        throw std::runtime_error("tuple_from_edge_id failed");
    }
    Tuple tuple = Tuple(lvid, leid, lfid, global_id);
    // accessor as parameter
    assert(is_ccw(tuple)); // is_ccw also checks for validity
    return tuple;
}
inline Tuple get_tuple_from_simplex_local_face_id(int8_t local_id, int64_t global_id)
{
    const auto& arr = autogen::tet_mesh::auto_3d_table_complete_face[local_id];
    const auto& [lvid, leid, lfid] = arr;
    assert(lfid == local_id);


    if (lvid < 0 || leid < 0 || lfid < 0) {
        throw std::runtime_error("tuple_from_face_id failed");
    }
    Tuple tuple = Tuple(lvid, leid, lfid, global_id);
    // accessor as parameter
    assert(is_ccw(tuple)); // is_ccw also checks for validity
    return tuple;
}
inline Tuple
get_tuple_from_simplex_local_id(PrimitiveType pt, int8_t local_id, int64_t global_fid)
{
    switch (pt) {
    case PrimitiveType::Vertex: return get_tuple_from_simplex_local_vertex_id(local_id, global_fid);
    case PrimitiveType::Edge: return get_tuple_from_simplex_local_edge_id(local_id, global_fid);
    case PrimitiveType::Triangle: return get_tuple_from_simplex_local_face_id(local_id, global_fid);
    default:
    case PrimitiveType::Tetrahedron: assert(false); return {};
    }
}
} // namespace wmtk::autogen::tet_mesh
