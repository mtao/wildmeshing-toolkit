#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {

// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart apply_simplex_involution(
    const dart::Dart& action,
    PrimitiveType pt,
    PrimitiveType opt,
    int8_t target_index, // ignored if pt == opt, otherwise it's the index in min(pt,opt) in simplex
                         // (opt,pt)
    const dart::Dart& source);

} // namespace wmtk::dart::utils
