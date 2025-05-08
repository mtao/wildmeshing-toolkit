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
int8_t get_permutation(const std::array<int64_t, N>& from, const std::array<int64_t, N>& to)
{
    PrimitiveType pt = get_primitive_type_from_id(N - 1);

    const auto& sd = SimplexDart::get_singleton(pt);
    int8_t ap = from_vertex_permutation(from);
    int8_t bp = from_vertex_permutation(to);

    const auto a = wmtk::dart::utils::get_local_vertex_permutation(pt, ap);
    const auto b = wmtk::dart::utils::get_local_vertex_permutation(pt, bp);

    spdlog::warn("From perm: {} to perm: {}", fmt::join(a, ","), fmt::join(b, ","));

    int8_t ret = sd.product(sd.inverse(ap), bp);

    assert(bp == sd.act(ap, ret));

    return ret;
}
} // namespace wmtk::dart::utils
