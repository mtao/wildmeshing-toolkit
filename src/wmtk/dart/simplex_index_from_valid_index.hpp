#pragma once
#include <wmtk/PrimitiveType.hpp>

// NOTE: this header primarily exists to simplify unit testing, not really for use
namespace wmtk::dart{
int8_t simplex_index_from_valid_index(
    PrimitiveType mesh_type,
    int8_t valid_tuple_index,
    PrimitiveType simplex_type);

} // namespace wmtk::autogen

