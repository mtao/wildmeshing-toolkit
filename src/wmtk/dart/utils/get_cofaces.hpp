#pragma once

#include <span>
#include "wmtk/PrimitiveType.hpp"

namespace wmtk::dart::utils {
std::vector<int8_t> get_cofaces(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType coface_pt);

std::vector<int8_t> get_coface_ids(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType coface_pt);
} // namespace wmtk::dart::utils
