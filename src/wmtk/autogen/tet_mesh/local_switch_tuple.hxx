#pragma once
#include <cassert>
#include <stdexcept>
#include "autogenerated_tables.hpp"
#include "local_id_table_offset.hpp"
#include "tuple_from_valid_index.hpp"
#include "valid_index_from_tuple.hpp"


namespace wmtk::autogen::tet_mesh {
inline Tuple local_switch_tuple(const Tuple& tuple, PrimitiveType pt)
{
    const int64_t offset = local_id_table_offset(tuple);


    const int64_t global_cid = tuple.global_cid();
    switch (pt) {
    case PrimitiveType::Vertex:
        return Tuple(
            auto_3d_table_vertex[offset][0],
            auto_3d_table_vertex[offset][1],
            auto_3d_table_vertex[offset][2],
            global_cid);

    case PrimitiveType::Edge:
        return Tuple(
            auto_3d_table_edge[offset][0],
            auto_3d_table_edge[offset][1],
            auto_3d_table_edge[offset][2],
            global_cid);
    case PrimitiveType::Triangle:
        return Tuple(
            auto_3d_table_face[offset][0],
            auto_3d_table_face[offset][1],
            auto_3d_table_face[offset][2],
            global_cid);

    case PrimitiveType::Tetrahedron:
    default: assert(false); // "Tuple switch: Invalid primitive type"
    }
    return Tuple();
}

inline Tuple local_switch_tuple(const Tuple& t, int8_t valid_tuple_index)
{
    int8_t input_index = valid_index_from_tuple(t);
    const int8_t product_result = auto_valid_switch_product_table[input_index][valid_tuple_index];
    return tuple_from_valid_index(t.global_cid(), product_result);
}

namespace internal {
inline int8_t switch_primitive_to_valid_tuple_index(wmtk::PrimitiveType pt)
{
    return auto_valid_tuple_switch_indices[get_primitive_type_id(pt)];
}
inline int8_t identity_valid_tuple_index()
{
    return switch_primitive_to_valid_tuple_index(wmtk::PrimitiveType::Tetrahedron);
}
} // namespace internal
} // namespace wmtk::autogen::tet_mesh
