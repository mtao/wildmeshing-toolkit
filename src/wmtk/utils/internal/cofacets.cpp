
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <map>
#include <set>
#if defined(_cpp_lib_span)
#include <span>
#endif
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/from_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_canonical_subdart_permutation.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/permute.hpp>
#include <wmtk/utils/DisjointSet.hpp>
#include "compactify_eigen_indices.hpp"


#include "IndexSimplexMapper.hpp"


namespace wmtk::utils::internal {
template <int8_t SimplexDim, int8_t FaceDim>
std::map<int64_t, std::set<dart::Dart>> cofacets(const IndexSimplexMapper& ism)
{
    std::map<int64_t, std::set<dart::Dart>> coboundary;
    auto S = ism.template simplices<SimplexDim>();
    for (int j = 0; j < S.size(); ++j) {
        const auto& s = S.at(j);
        std::array<int64_t, FaceDim> r;
        std::copy(s.begin(), s.begin() + FaceDim, r.begin());
        std::array<int64_t, SimplexDim - FaceDim> tail;
        std::copy(s.begin() + FaceDim, s.end(), tail.begin());

        std::array<int64_t, SimplexDim + 1> ss = s;
        std::array<int8_t, SimplexDim + 1> local_permutation;
        std::iota(local_permutation.begin(), local_permutation.end(), 0);

        auto add = [&]() {
            int64_t index = ism.get_index(r);

            std::array<int64_t, FaceDim> sorted = r;
            std::sort(sorted.begin(), sorted.end());
            int8_t p = wmtk::dart::utils::get_canonical_subdart_permutation(ss, sorted);
            auto myd = wmtk::dart::Dart(j, p);
            coboundary[index].emplace(myd);
        };

        if constexpr (SimplexDim == FaceDim + 1) {
            add();

            for (int k = 0; k < SimplexDim; ++k) {
                int64_t old = r[k];
                r[k] = tail[0];
                add();
                r[k] = old;
            }
        } else {
            std::array<int64_t, SimplexDim> arr = ss;

            do {
                const bool start_sorted = std::is_sorted(arr.begin(), arr.begin() + FaceDim);
                const bool end_sorted = std::is_sorted(arr.begin() + FaceDim, arr.end());
                if (start_sorted && end_sorted) {
                    std::copy(s.begin(), s.begin() + FaceDim, r.begin());
                    add();
                }
            } while (std::next_permutation(arr.begin(), arr.end()));
        }
    }
    return coboundary;
}
template <int8_t Dim>
std::map<int64_t, std::set<dart::Dart>> coboundary(const IndexSimplexMapper& ism)
{
    std::map<int64_t, std::set<dart::Dart>> coboundary;
    auto S = ism.template simplices<Dim>();
    for (int j = 0; j < S.size(); ++j) {
        const auto& s = S.at(j);
        std::array<int64_t, Dim> r;
        std::copy(s.begin(), s.end() - 1, r.begin());
        int64_t last = s.back();

        std::array<int64_t, Dim + 1> ss = s;
        std::array<int8_t, Dim + 1> local_permutation;
        std::iota(local_permutation.begin(), local_permutation.end(), 0);

        auto add = [&]() {
            int64_t index = ism.get_index(r);

            std::array<int64_t, Dim> sorted = r;
            std::sort(sorted.begin(), sorted.end());
            int8_t p = wmtk::dart::utils::get_canonical_subdart_permutation(ss, sorted);
            auto myd = wmtk::dart::Dart(j, p);
            coboundary[index].emplace(myd);
        };

        add();

        for (int k = 0; k < Dim; ++k) {
            int64_t old = r[k];
            r[k] = last;
            add();
            r[k] = old;
        }
    }
    return coboundary;
}
std::map<int64_t, std::set<dart::Dart>> coboundary(
    const IndexSimplexMapper& ism,
    int8_t dim)
{
    switch (dim) {
    case 0: return coboundary<0>(ism);
    case 1: return coboundary<1>(ism);
    case 2: return coboundary<2>(ism);
    case 3: return coboundary<3>(ism);
    default: assert(false); return {};
    }
}
template std::map<int64_t, std::set<dart::Dart>> cofacets<1, 0>(
    const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<2, 1>(
    const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<2, 0>(
    const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<3, 0>(
    const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<3, 1>(
    const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<3, 2>(
    const IndexSimplexMapper& ism);

} // namespace wmtk::utils::internal
