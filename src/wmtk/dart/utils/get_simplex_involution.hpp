#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {

// pt >= opt
int8_t get_simplex_involution_downwards(PrimitiveType pt, int8_t a, PrimitiveType opt, int8_t b);


// maps (pt,a) to (opt,oa) then (opt,b)
int8_t
get_simplex_involution(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b);

std::pair<int8_t, int8_t>
get_simplex_involution_pair(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b);


// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart get_simplex_involution(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b);

std::pair<wmtk::dart::Dart, wmtk::dart::Dart> get_simplex_involution_pair(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b);
} // namespace wmtk::dart::utils
