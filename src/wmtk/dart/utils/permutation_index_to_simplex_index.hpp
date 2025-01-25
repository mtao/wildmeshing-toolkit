#pragma once
#include <cstdint>
#include <wmtk/PrimitiveType.hpp>

namespace wmtk::dart::utils {
int8_t permutation_index_to_simplex_index(
    PrimitiveType mesh_type,
    int8_t permutation_index,
    PrimitiveType target_type);

}
