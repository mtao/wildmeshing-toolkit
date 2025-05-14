#pragma once


#include <map>
#include <wmtk/Types.hpp>
#include "from_local_vertex_permutation.hpp"
#include "get_permutation.hpp"
namespace wmtk::dart::utils {

template <typename IntLike, size_t D, size_t d>
std::array<int8_t, D> get_canonical_subdart_permutation_indices(
    const std::array<IntLike, D>& S,
    const std::array<IntLike, d>& s)
{
    if constexpr (D == d) {
        return get_permutation(S, s);
    } else {
        // sorted copy for the sake of checking for elements in s
        std::array<IntLike, d> sorted = s;
        std::sort(sorted.begin(), sorted.end());

        std::map<IntLike, int8_t> map = detail::local_index_map(S);
        std::array<int8_t, D> R;
        size_t sup_index = d;
        // std::vector<IntLike> I(indices.size());
        for (int8_t j = 0; j < S.size(); ++j) {
            const IntLike t = S[j];
            if (!std::binary_search(sorted.begin(), sorted.end(), t)) {
                R[sup_index++] = j;
            }
        }
        for (size_t j = 0; j < s.size(); ++j) {
            R[j] = map.at(s[j]);
        }
        return R;
    }
}


template <typename IntLike, size_t D, size_t d>
int8_t get_canonical_subdart_permutation(
    const std::array<IntLike, D>& S,
    const std::array<IntLike, d>& s)
{
    const auto r = get_canonical_subdart_permutation_indices(S, s);
    return from_local_vertex_permutation(r);
}
} // namespace wmtk::dart::utils
