#pragma once
#include <span>
#include <wmtk/dart/SimplexDart.hpp>

namespace wmtk::dart::utils {

std::span<const int8_t> get_canonical_simplices(const SimplexDart& sd, PrimitiveType pt);
std::span<const int8_t> get_canonical_simplices(const PrimitiveType mesh_pt, PrimitiveType pt);
} // namespace wmtk::dart::utils
