#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart::utils {

    // pt >= opt
wmtk::dart::Dart get_simplex_involution_downwards(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b);


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
