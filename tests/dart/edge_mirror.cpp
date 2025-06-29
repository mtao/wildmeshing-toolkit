#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>

#include <wmtk/dart/utils/edge_mirror.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/opposite.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "tools/all_valid_local_tuples.hpp"
#include "tools/darts_using_faces.hpp"
#include "utils/canonical_darts.hpp"
using namespace wmtk;
using namespace wmtk::dart;

using namespace wmtk::tests::dart::utils;

namespace {
void print(const SimplexDart& sd, const Dart& a, const Dart& b)
{
    spdlog::info(
        "{} {}",
        fmt::join(
            wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(), a.permutation()),
            ","),
        fmt::join(
            wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(), b.permutation()),
            ","));
}
} // namespace

TEST_CASE("dart_opposite", "[tuple]")
{
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Edge);
        CHECK(wmtk::dart::utils::opposite(sd, d01) == d10);
        CHECK(wmtk::dart::utils::opposite(sd, d10) == d01);
    }
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Triangle);
        spdlog::info(
            "{}",
            fmt::join(
                wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(), sd.opposite()),
                ","));
        print(sd, wmtk::dart::utils::opposite(sd, d012), d210);
        print(sd, wmtk::dart::utils::opposite(sd, d102), d201);
        print(sd, wmtk::dart::utils::opposite(sd, d021), d120);
        print(sd, wmtk::dart::utils::opposite(sd, d120), d021);
        print(sd, wmtk::dart::utils::opposite(sd, d201), d102);
        print(sd, wmtk::dart::utils::opposite(sd, d210), d012);
        CHECK(wmtk::dart::utils::opposite(sd, d012) == d210);
        CHECK(wmtk::dart::utils::opposite(sd, d102) == d201);
        CHECK(wmtk::dart::utils::opposite(sd, d021) == d120);
        CHECK(wmtk::dart::utils::opposite(sd, d120) == d021);
        CHECK(wmtk::dart::utils::opposite(sd, d201) == d102);
        CHECK(wmtk::dart::utils::opposite(sd, d210) == d012);
    }
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
        CHECK(wmtk::dart::utils::opposite(sd, d0123) == d3210);
        CHECK(wmtk::dart::utils::opposite(sd, d1023) == d3201);
        CHECK(wmtk::dart::utils::opposite(sd, d0213) == d3120);
        CHECK(wmtk::dart::utils::opposite(sd, d1203) == d3021);
        CHECK(wmtk::dart::utils::opposite(sd, d2013) == d3102);
        CHECK(wmtk::dart::utils::opposite(sd, d2103) == d3012);

        CHECK(wmtk::dart::utils::opposite(sd, d0132) == d2310);
        CHECK(wmtk::dart::utils::opposite(sd, d1032) == d2301);
        CHECK(wmtk::dart::utils::opposite(sd, d0312) == d2130);
        CHECK(wmtk::dart::utils::opposite(sd, d1302) == d2031);
        CHECK(wmtk::dart::utils::opposite(sd, d3012) == d2103);
        CHECK(wmtk::dart::utils::opposite(sd, d3102) == d2013);

        CHECK(wmtk::dart::utils::opposite(sd, d0321) == d1230);
        CHECK(wmtk::dart::utils::opposite(sd, d3021) == d1203);
        CHECK(wmtk::dart::utils::opposite(sd, d0231) == d1320);
        CHECK(wmtk::dart::utils::opposite(sd, d3201) == d1023);
        CHECK(wmtk::dart::utils::opposite(sd, d2031) == d1302);
        CHECK(wmtk::dart::utils::opposite(sd, d2301) == d1032);

        CHECK(wmtk::dart::utils::opposite(sd, d3012) == d2103);
        CHECK(wmtk::dart::utils::opposite(sd, d0312) == d2130);
        CHECK(wmtk::dart::utils::opposite(sd, d3102) == d2013);
        CHECK(wmtk::dart::utils::opposite(sd, d0132) == d2310);
        CHECK(wmtk::dart::utils::opposite(sd, d1302) == d2031);
        CHECK(wmtk::dart::utils::opposite(sd, d1032) == d2301);
    }
}
TEST_CASE("dart_edge_mirror", "[tuple]")
{
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Edge);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d01) == d10);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d10) == d01);
    }
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Triangle);
        print(sd, wmtk::dart::utils::edge_mirror(sd, d012), d102);
        print(sd, wmtk::dart::utils::edge_mirror(sd, d102), d012);
        print(sd, wmtk::dart::utils::edge_mirror(sd, d021), d120);
        print(sd, wmtk::dart::utils::edge_mirror(sd, d120), d021);
        print(sd, wmtk::dart::utils::edge_mirror(sd, d201), d210);
        print(sd, wmtk::dart::utils::edge_mirror(sd, d210), d201);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012) == d102);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d102) == d012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d021) == d120);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d120) == d021);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d201) == d210);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d210) == d201);
    }
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1023) == d0123);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0123) == d1023);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1203) == d0213);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0213) == d1203);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2103) == d2013);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2013) == d2103);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d1032) == d0132);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0132) == d1032);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1302) == d0312);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0312) == d1302);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3102) == d3012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3012) == d3102);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d1320) == d0321);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3120) == d3021);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1230) == d0231);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3210) == d3201);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2130) == d2031);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2310) == d2301);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d3102) == d3012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1302) == d0312);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3012) == d3102);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1032) == d0132);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0312) == d1302);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0132) == d1032);
    }
}

TEST_CASE("dart_edge_mirror_with_edge", "[tuple]")
{
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Edge);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d01, d10) == d01);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d10, d10) == d01);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d01, d01) == d10);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d10, d01) == d10);
    }
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Triangle);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012, d012) == d102);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012, d102) == d012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012, d021) == d120);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012, d120) == d021);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012, d201) == d210);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d012, d210) == d201);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d102, d012) == d102);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d102, d102) == d012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d102, d021) == d120);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d102, d120) == d021);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d102, d201) == d210);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d102, d210) == d201);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d201, d012) == d210);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d201, d102) == d120);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d201, d021) == d201);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d201, d120) == d102);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d201, d201) == d021);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d201, d210) == d012);

        // CHECK(wmtk::dart::utils::edge_mirror(sd, d021, d012) == d210);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d021, d102) == d120);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d021, d021) == d201);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d021, d120) == d102);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d021, d201) == d021);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d021, d210) == d012);

        // CHECK(wmtk::dart::utils::edge_mirror(sd, d120, d012) == d021);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d120, d102) == d201);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d120, d021) == d012);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d120, d120) == d210);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d120, d201) == d102);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d120, d210) == d120);

        // CHECK(wmtk::dart::utils::edge_mirror(sd, d210, d012) == d210);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d210, d102) == d120);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d210, d021) == d201);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d210, d120) == d102);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d210, d201) == d021);
        // CHECK(wmtk::dart::utils::edge_mirror(sd, d210, d210) == d012);
    }
    {
        const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1023) == d0123);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0123) == d1023);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1203) == d0213);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0213) == d1203);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2103) == d2013);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2013) == d2103);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d1032) == d0132);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0132) == d1032);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1302) == d0312);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0312) == d1302);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3102) == d3012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3012) == d3102);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d1320) == d0321);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3120) == d3021);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1230) == d0231);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3210) == d3201);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2130) == d2031);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d2310) == d2301);

        CHECK(wmtk::dart::utils::edge_mirror(sd, d3102) == d3012);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1302) == d0312);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d3012) == d3102);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d1032) == d0132);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0312) == d1302);
        CHECK(wmtk::dart::utils::edge_mirror(sd, d0132) == d1032);
    }
}
