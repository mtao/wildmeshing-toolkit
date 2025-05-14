#pragma once

#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
namespace wmtk::dart::utils {

// returns the permutation represented by a given permutation index
VectorX<int8_t>::ConstMapType get_local_vertex_permutation(
    PrimitiveType pt,
    int8_t permutation_index);


// returns the permutation reprsented by a given permutation index, using the input simplex
// dimension as a compile-time parametesr
template <size_t SimplexDimension>
std::array<int8_t, SimplexDimension + 1> get_local_vertex_permutation(int8_t permutation_index);
} // namespace wmtk::dart::utils
