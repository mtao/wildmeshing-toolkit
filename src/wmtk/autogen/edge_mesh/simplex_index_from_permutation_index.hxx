#pragma once
// DO NOT MODIFY, autogenerated from the /scripts directory
#include "autogenerated_tables.hpp"
#include "local_id_table_offset.hpp"
#include "simplex_index_from_permutation_index.hpp"
namespace wmtk::autogen::edge_mesh {
inline int8_t simplex_index_from_permutation_index(
    int8_t permutation_index,
    wmtk::PrimitiveType type)
{
    const int8_t type_index = get_primitive_type_id(type);
    return auto_valid_tuples[permutation_index][type_index];
}

} // namespace wmtk::autogen::edge_mesh