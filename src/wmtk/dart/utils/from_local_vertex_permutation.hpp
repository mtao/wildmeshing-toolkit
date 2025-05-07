#pragma once

#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
#include <array>
namespace wmtk::dart::utils {


// finds the index of a permutation encoded by indices {0...N}
int8_t from_local_vertex_permutation(
    PrimitiveType pt,
    Eigen::Ref<const VectorX<int8_t>> permutation);
int8_t from_local_vertex_permutation(Eigen::Ref<const VectorX<int8_t>> permutation);

int8_t from_vertex_permutation(Eigen::Ref<const VectorX<int64_t>> indices);


int8_t from_vertex_permutation(PrimitiveType pt, Eigen::Ref<const VectorX<int64_t>> indices);


template <size_t N>
int8_t from_local_vertex_permutation(const std::array<int8_t, N>& indices);

template <size_t N>
int8_t from_vertex_permutation(const std::array<int64_t,N>& indices);
} // namespace wmtk::dart::utils
