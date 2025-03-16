#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {

// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart apply_simplex_involution(
    const dart::Dart& involution,
    PrimitiveType pt,
    PrimitiveType opt,
    int8_t target_index, // ignored if pt == opt, otherwise it's the index in min(pt,opt) in simplex
                         // (opt,pt)
    const dart::Dart& source);

wmtk::dart::Dart apply_simplex_involution(
    int8_t involution,
    PrimitiveType pt,
    PrimitiveType opt,
    int8_t target_index, // ignored if pt == opt, otherwise it's the index in min(pt,opt) in simplex
                         // (opt,pt)
    int8_t source);

wmtk::dart::Dart apply_simplex_involution(
    const dart::Dart& involution,
    PrimitiveType pt,
    PrimitiveType opt,
    const dart::Dart& source);

wmtk::dart::Dart apply_simplex_involution(
    int8_t involution,
    PrimitiveType pt,
    PrimitiveType opt,
    int8_t source);

} // namespace wmtk::dart::utils
