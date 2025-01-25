#pragma once
#include <wmtk/PrimitiveType.hpp>

namespace wmtk::dart::utils {
auto simplex_index_from_permutation_index(
    const PrimitiveType mesh_type,
    int8_t permutation_index,
    wmtk::PrimitiveType type) -> int8_t;

}

