#pragma once

#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
namespace wmtk::dart::utils {


// TODO: this is more for debugging until a faster implementation is built
int8_t from_local_vertex_permutation(
    PrimitiveType pt,
    Eigen::Ref<const VectorX<int8_t>> permutation);
} // namespace wmtk::dart::utils
