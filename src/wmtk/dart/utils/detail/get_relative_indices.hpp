#pragma once
#include <array>
#include <cstdint>
#include <map>

namespace wmtk::dart::utils::detail {


template <typename IntLike, size_t D>
std::map<IntLike, int8_t> local_index_map(const std::array<IntLike, D>& S)
{
    std::map<IntLike, int8_t> map;
    for (int8_t j = 0; j < S.size(); ++j) {
        const IntLike t = S[j];
        map[t] = j;
    }
    assert(map.size() == D);
    return map;
}


template <typename IntLike, size_t D, size_t d>
std::array<int8_t, D> get_relative_indices(
    const std::array<IntLike, D>& S,
    const std::array<IntLike, d>& s)
{
    auto map = local_index_map(S);
    std::array<int8_t, d> R;
    for (size_t j = 0; j < s.size(); ++j) {
        R[j] = map.at(s[j]);
    }
    return R;
}
} // namespace wmtk::dart::utils::detail
