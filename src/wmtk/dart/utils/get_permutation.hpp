#pragma once

#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
#include "from_local_vertex_permutation.hpp"
#include <wmtk/dart/find_local_dart_action.hpp>
namespace wmtk::dart::utils {

    // Evaluates the matrix
    template <size_t N>
    int8_t get_permutation(const std::array<int64_t, N>& from, const std::array<int64_t, N>& to)
{
    PrimitiveType pt = get_primitive_type_from_id(N - 1);

    using MapType = typename Eigen::Vector<int64_t, N>::ConstMapType;
    MapType A(from.data());
    MapType B(to.data());
    const auto& sd = SimplexDart::get_singleton(pt);
    int8_t ap = from_vertex_permutation(pt, A);
    int8_t bp = from_vertex_permutation(pt, B);
    return find_local_dart_action(sd,ap,bp);
}
} // namespace wmtk::dart::utils
