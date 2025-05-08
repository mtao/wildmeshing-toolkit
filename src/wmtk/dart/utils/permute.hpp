#pragma once

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include "from_local_vertex_permutation.hpp"
namespace wmtk::dart::utils {

// Evaluates the matrix
template <size_t N>
std::array<int64_t, N> permute(const std::array<int64_t, N>& from, int8_t permutation)
{
    PrimitiveType pt = get_primitive_type_from_id(N - 1);

    const auto& sd = SimplexDart::get_singleton(pt);
    int8_t ap = from_vertex_permutation(from);

    const auto a = wmtk::dart::utils::get_local_vertex_permutation(pt, permutation);

    std::array<int64_t, N> r;
    for(size_t j = 0; j < N; ++j) {
        r[j] = from[a[j]];
    }

    return r;
}
} // namespace wmtk::dart::utils
