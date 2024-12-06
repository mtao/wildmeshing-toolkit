#pragma once
#include <cassert>
#include <wmtk/Tuple.hpp>
#include "autogenerated_tables.hpp"
#if !defined(_NDEBUG)
#include "is_ccw.hpp"
#endif

namespace wmtk::autogen::tri_mesh {
inline Tuple get_tuple_from_simplex_local_vertex_id(int8_t local_id, int64_t global_id)
{
    assert(local_id >= 0);
    assert(local_id < 3);

    const auto& arr = autogen::tri_mesh::auto_2d_table_complete_vertex[local_id];
    const auto& [lvid, leid] = arr;
    Tuple tuple = Tuple(lvid, leid, -1, global_id);
    return tuple;
}
inline Tuple get_tuple_from_simplex_local_edge_id(int8_t local_id, int64_t global_id)
{
    assert(local_id >= 0);
    assert(local_id < 3);
    const auto& arr = autogen::tri_mesh::auto_2d_table_complete_edge[local_id];
    const auto& [lvid, leid] = arr;
    Tuple tuple = Tuple(lvid, leid, -1, global_id);
    return tuple;
}
inline Tuple get_tuple_from_simplex_local_id(PrimitiveType pt, int8_t local_id, int64_t global_fid)
{
    switch (pt) {
    case PrimitiveType::Vertex: return get_tuple_from_simplex_local_vertex_id(local_id, global_fid);
    case PrimitiveType::Edge: return get_tuple_from_simplex_local_edge_id(local_id, global_fid);
    default:
    case PrimitiveType::Triangle:
    case PrimitiveType::Tetrahedron: assert(false); return {};
    }
}
} // namespace wmtk::autogen::tri_mesh
