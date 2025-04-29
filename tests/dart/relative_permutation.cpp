#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/from_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
using namespace wmtk;
using namespace wmtk::dart;
using namespace wmtk::tests::dart::utils;


template <size_t N>
void run(const std::array<size_t, N>& arr)
{
    const SimplexDart& sb = wmtk::dart::utils::get_simplex_dart(N - 1);
    auto pt = sb.simplex_type();

    for (int8_t aperm : sa.permutation_indices()) {
        auto perm = wmtk::dart::utils::get_local_vertex_permutation(pt, aperm);
        REQUIRE(perm.size() == N);
        std::array<size_t, N> permed;
        for (size_t j = 0; j < N; ++j) {
            permed[j] = arr[perm(j)];
        }

        wmtk::dart::utils::get_permutation(arr, permed);
    }
}


TEST_CASE("get_relative_permutation", "[permutation]")
{
    std::array<int64_t, 3> a{{10, 5, 20}};
}
