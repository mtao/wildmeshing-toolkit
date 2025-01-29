#pragma once

#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
namespace wmtk::dart::utils {

VectorX<int8_t>::ConstMapType get_local_vertex_permutation(
    PrimitiveType pt,
    int8_t permutation_index);
}
