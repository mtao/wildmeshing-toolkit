#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {

// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart apply_simplex_involution(
    PrimitiveType a_pt,
    const dart::Dart& a,
    int8_t a_local_index,
    PrimitiveType opt,
    const dart::Dart& involution);

} // namespace wmtk::dart::utils
