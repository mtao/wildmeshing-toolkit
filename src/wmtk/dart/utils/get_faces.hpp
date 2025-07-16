#pragma once

#include <span>
#include "wmtk/PrimitiveType.hpp"

namespace wmtk::dart::utils {
std::vector<int8_t> get_faces(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType face_pt);

std::vector<int8_t> get_face_ids(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType face_pt);
} // namespace wmtk::dart::utils
