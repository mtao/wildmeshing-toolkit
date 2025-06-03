#pragma once
#include <wmtk/dart/SimplexDart.hpp>

namespace wmtk::dart::utils {

// Given a sd_pt-dart, returns a pt-dart "r" and base_pt-dart "o", where "r" is the relative
// orientation compared to the canonical "o"-simplex
std::pair<int8_t, int8_t>
local_simplex_decomposition(const SimplexDart& sd, PrimitiveType pt, int8_t permutation);
} // namespace wmtk::dart::utils
