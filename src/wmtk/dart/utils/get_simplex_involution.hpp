#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {

// pt >= opt
//int8_t get_simplex_involution_downwards(PrimitiveType pt, int8_t a, PrimitiveType opt, int8_t b);


// encodes the map from (pt, a) to (opt,oa) as ( oa L_{pt->opt}(a^{-1}) * canonical_supdart(a))
// where L_{pt->opt} lifts the permutation a from pt to opt (via concatenation with subdart)
// maps (pt,a) to (opt,oa) then (opt,b).
// Note that due to orthogonality we see that
// (oa L_{pt->opt}(a^{-1}))
//and 
// canonical_supdart(a)
// can be separated. As such, we can map (pt,b) to (opt,b) by
// always separating this permutation into
// (oa L_{pt->opt}(a^{-1})) L_{pt->opt}(b) canonical_supdart(a)
// = (oa L_{pt->opt}(a^{-1} b) canonical_supdart(a)
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
