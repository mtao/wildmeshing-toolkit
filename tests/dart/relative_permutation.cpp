#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/get_simplex_dart.hpp>
#include <wmtk/dart/utils/from_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_permutation.hpp>
#include "utils/canonical_darts.hpp"
using namespace wmtk;
using namespace wmtk::dart;


template <size_t N>
void run()
{
    PrimitiveType pt = get_primitive_type_from_id(N - 1);
    const SimplexDart& sb = wmtk::dart::SimplexDart::get_singleton(pt);

    for (int8_t aperm = 0; aperm < sb.size(); ++aperm) {
        const auto Aperm = wmtk::dart::utils::get_local_vertex_permutation(pt, aperm);
        REQUIRE(Aperm.size() == N);

        for (int8_t bperm = 0; bperm < sb.size(); ++bperm) {
            const auto Bperm = wmtk::dart::utils::get_local_vertex_permutation(pt, bperm);
            REQUIRE(Bperm.size() == N);
            std::array<int64_t, N> permed;

            int8_t cperm = sb.product(aperm, bperm);
            const auto Cperm = wmtk::dart::utils::get_local_vertex_permutation(pt, cperm);
            spdlog::warn(
                "{} = {} x {}",
                fmt::join(Aperm, ","),
                fmt::join(Bperm, ","),
                fmt::join(Cperm, ","));
            for (size_t j = 0; j < N; ++j) {
                CHECK(Cperm(j) == Aperm(Bperm(j)));
            }
        }
    }
}

template <size_t N>
void run(const std::array<int64_t, N>& arr)
{
    PrimitiveType pt = get_primitive_type_from_id(N - 1);
    const SimplexDart& sb = wmtk::dart::SimplexDart::get_singleton(pt);

    for (int8_t aperm = 0; aperm < sb.size(); ++aperm) {
        const auto perm = wmtk::dart::utils::get_local_vertex_permutation(pt, aperm);
        REQUIRE(perm.size() == N);
        std::array<int64_t, N> permed;
        for (size_t j = 0; j < N; ++j) {
            permed[j] = arr[perm(j)];
        }

        int8_t bperm = wmtk::dart::utils::get_permutation(arr, permed);
        CHECK(aperm == bperm);
    }
}

TEST_CASE("simple_product_example", "[permutation]")
{
    std::array<int64_t,4> Q{{3,2,1,0}};
    std::array<int64_t,4> P{{1,3,0,2}};
    // Q * P
    // ==
    //{0,1,2,3} {0,1,2,3}
    //{3,2,1,0} {1,3,0,2}
    // ==
    //{1,3,0,2} {0,1,2,3}
    //{2,0,3,1} {1,3,0,2}
    //
    std::array<int64_t,4> R{{2,0,3,1}};

    for(size_t j = 0; j < 4; ++j) {
        CHECK(R[j] == Q[P[j]]);
    }
    PrimitiveType pt =wmtk::PrimitiveType::Tetrahedron;
    const SimplexDart& sb = wmtk::dart::SimplexDart::get_singleton(pt);

    const int8_t q= wmtk::dart::utils::from_local_vertex_permutation(Q);
    const int8_t r= wmtk::dart::utils::from_local_vertex_permutation(R);
    const int8_t p= wmtk::dart::utils::from_local_vertex_permutation(P);

}

TEST_CASE("get_relative_permutation", "[permutation]")
{
    run(std::array<int64_t, 3>{{10, 5, 20}});
    run(std::array<int64_t, 2>{{10, 20}});
    run(std::array<int64_t, 4>{{10, 0, 5, 20}});

    // run<1>();
    run<2>();
    run<3>();
}

TEST_CASE("composition", "[permutation]")
{
    using namespace wmtk::tests::dart::utils;
    constexpr static PrimitiveType pt = PrimitiveType::Triangle;
    const auto& sd = SimplexDart::get_singleton(pt);
    const int8_t p012 = d012.permutation();
    const int8_t p021 = d021.permutation();
    const int8_t p102 = d102.permutation();
    const int8_t p120 = d120.permutation();
    const int8_t p201 = d201.permutation();
    const int8_t p210 = d210.permutation();

    // sd.product(p201,
}
