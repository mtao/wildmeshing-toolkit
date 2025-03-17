#pragma once
#include <wmtk/dart/SimplexDart.hpp>
#include "get_canonical_supdart.hpp"

namespace wmtk::dart::utils {

inline int8_t
get_canonical_simplex_orientation(const SimplexDart& sd, PrimitiveType pt, int8_t permutation)
{
    int8_t canonical_orientation = get_canonical_supdart(sd,pt,permutation);


    return sd.product(permutation, sd.inverse(canonical_orientation));
}
} // namespace wmtk::dart::utils
