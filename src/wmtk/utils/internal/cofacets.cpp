
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

namespace {
template <int8_t dim, int8_t facet_dim>
struct combinations;

template <>
struct combinations<1, 1>
{
    constexpr static std::array<std::array<int8_t, 1>, 1> values = {{{{0}}}};
};
template <>
struct combinations<2, 1>
{
    constexpr static std::array<std::array<int8_t, 1>, 2> values = {{
        {{0}},
        {{1}},
    }};
};
template <>
struct combinations<3, 1>
{
    constexpr static std::array<std::array<int8_t, 1>, 3> values = {{
        {{0}},
        {{1}},
        {{2}},
    }};
};
template <>
struct combinations<3, 2>
{
    constexpr static std::array<std::array<int8_t, 2>, 3> values = {{
        {{0, 1}},
        {{0, 2}},
        {{1, 2}},
    }};
};
template <>
struct combinations<4, 1>
{
    constexpr static std::array<std::array<int8_t, 1>, 4> values = {{
        {{0}},
        {{1}},
        {{2}},
    }};
};
template <>
struct combinations<4, 2>
{
    constexpr static std::array<std::array<int8_t, 2>, 6> values = {{
        {{0, 1}},
        {{0, 2}},
        {{1, 2}},
        {{0, 3}},
        {{1, 3}},
        {{2, 3}},
    }};
};
template <>
struct combinations<4, 3>
{
    constexpr static std::array<std::array<int8_t, 3>, 4> values = {{
        {{0, 1, 2}},
        {{1, 2, 3}},
        {{0, 2, 3}},
        {{0, 1, 3}},
    }};
};
} // namespace

namespace wmtk::utils::internal {
template <int8_t SimplexDim, int8_t FaceDim>
std::map<int64_t, std::set<dart::Dart>> cofacets(const IndexSimplexMapper& ism)
{
    std::map<int64_t, std::set<dart::Dart>> coboundary;
    auto S = ism.template simplices<SimplexDim>();
    for (int j = 0; j < S.size(); ++j) {
        const auto& s = S.at(j);
        static_assert(std::tuple_size_v<std::decay_t<decltype(s)>> == SimplexDim + 1);
        std::array<int64_t, FaceDim + 1> r;
        std::copy(s.begin(), s.begin() + FaceDim, r.begin());

        std::array<int64_t, SimplexDim + 1> ss = s;
        std::array<int8_t, SimplexDim + 1> local_permutation;
        std::iota(local_permutation.begin(), local_permutation.end(), 0);

        auto add = [&]() {
            int64_t index = ism.get_index(r);

            std::array<int64_t, FaceDim + 1> sorted = r;
            std::sort(sorted.begin(), sorted.end());
            int8_t p = wmtk::dart::utils::get_canonical_subdart_permutation(ss, sorted);
            auto myd = wmtk::dart::Dart(j, p);
            coboundary[index].emplace(myd);
        };

        for (const auto& comb : combinations<SimplexDim + 1, FaceDim + 1>::values) {
            for (size_t k = 0; k < FaceDim + 1; ++k) {
                r[k] = s[comb[k]];
            }
            add();
        }
    }
    return coboundary;
}
template <int8_t Dim>
std::map<int64_t, std::set<dart::Dart>> coboundary(const IndexSimplexMapper& ism)
{
    return cofacets<Dim, Dim - 1>(ism);
    /*
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
    */
}

std::map<int64_t, std::set<dart::Dart>> coboundary(const IndexSimplexMapper& ism, int8_t dim)
{
    switch (dim) {
    case 1: return coboundary<1>(ism);
    case 2: return coboundary<2>(ism);
    case 3: return coboundary<3>(ism);
    default: assert(false); return {};
    }
}
template std::map<int64_t, std::set<dart::Dart>> cofacets<1, 0>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<2, 1>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<2, 0>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<3, 0>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<3, 1>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> cofacets<3, 2>(const IndexSimplexMapper& ism);

template std::map<int64_t, std::set<dart::Dart>> coboundary< 1>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> coboundary< 2>(const IndexSimplexMapper& ism);
template std::map<int64_t, std::set<dart::Dart>> coboundary< 3>(const IndexSimplexMapper& ism);

} // namespace wmtk::utils::internal
