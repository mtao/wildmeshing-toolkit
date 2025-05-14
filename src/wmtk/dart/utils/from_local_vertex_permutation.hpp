#pragma once
#include <algorithm>
#include <wmtk/dart/SimplexDart.hpp>
#include "detail/get_relative_indices.hpp"

#include <Eigen/Core>
#include <array>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
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
//
// template <size_t N>
// int8_t from_vertex_permutation(const std::array<int64_t, N>& indices);

template <typename IntLike, size_t N>
int8_t from_vertex_permutation(const std::array<IntLike, N>& indices)
{
    if constexpr (N != 1) {
        std::array<IntLike, N> sorted = indices;
        std::sort(sorted.begin(), sorted.end());
        std::array<int8_t, N> perm = detail::get_relative_indices(sorted, indices);
        PrimitiveType pt = get_primitive_type_from_id(indices.size() - 1);
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(pt);
        return from_local_vertex_permutation(perm);
    } else {
        return 0;
    }
}
} // namespace wmtk::dart::utils
