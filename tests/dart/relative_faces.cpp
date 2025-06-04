
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/apply_simplex_involution.hpp>
#include <wmtk/dart/utils/get_canonical_simplex_orientation.hpp>
#include <wmtk/dart/utils/get_canonical_subdart.hpp>
#include <wmtk/dart/utils/get_canonical_supdart.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_simplex_involution.hpp>
#include <wmtk/dart/utils/local_simplex_decomposition.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "utils/canonical_darts.hpp"
using namespace wmtk;
using namespace wmtk::dart;
using namespace wmtk::tests::dart::utils;


// takes in a source dart that maps to a target dart. checks that if we map a * source we get a *
// target
template <typename T>
void dart_map_checker(
    const SimplexDart& sd,
    const Dart& source,
    const SimplexDart& sd2,
    const Dart& target,
    int8_t lower_action,
    const T& all_lower_darts)
{
    REQUIRE(sd.simplex_type() <= sd2.simplex_type());
    spdlog::error(
        "Mapping {} -> {} with action {}",
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd.simplex_type(), source.permutation()),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), target.permutation()),
            ","),
        fmt::join(dart::utils::get_local_vertex_permutation(sd.simplex_type(), lower_action), ",")

    );

    auto [target_permutation, upper_simplex] = wmtk::dart::utils::local_simplex_decomposition(
        sd2,
        sd.simplex_type(),
        target.permutation());
    spdlog::error(
        "Target was decomposed to {} from supdart {} with action {} (global was {}). manual "
        "reconstruction is {}",
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), target.permutation()),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), upper_simplex),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd.simplex_type(), target_permutation),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd2.simplex_type(),
                sd.convert(target_permutation, sd2)),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd2.simplex_type(),
                sd2.product(upper_simplex, sd.convert(target_permutation, sd2))),
            ",")

    );

    REQUIRE(
        sd2.product(upper_simplex, sd.convert(target_permutation, sd2)) == target.permutation());
    // int8_t upper_simplex =
    //     wmtk::dart::utils::get_canonical_supdart(sd2, sd2.simplex_type(), target.permutation());


    int8_t upper_action = sd.convert(lower_action, sd2);
    // REQUIRE(sd2.product(upper_simplex, upper_action) == target.permutation());

    auto new_source = sd.act(source, lower_action);
    auto new_target = sd2.act(target, upper_action);

    spdlog::info(
        "Source products: {} x {} -> {} (got {})",
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd.simplex_type(), source.permutation()),
            ","),
        fmt::join(dart::utils::get_local_vertex_permutation(sd.simplex_type(), lower_action), ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd.simplex_type(),
                sd.act(source.permutation(), lower_action)),
            ","),

        fmt::join(
            dart::utils::get_local_vertex_permutation(sd.simplex_type(), new_source.permutation()),
            ",")

    );
    spdlog::info(
        "target products: {} x {} -> {} (got {})",
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), target.permutation()),
            ","),
        fmt::join(dart::utils::get_local_vertex_permutation(sd2.simplex_type(), upper_action), ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd2.simplex_type(),
                sd2.act(target.permutation(), upper_action)),
            ","),

        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), new_target.permutation()),
            ",")

    );


    spdlog::info(
        "With action we should have {} -> {}",
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd.simplex_type(), new_source.permutation()),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), new_target.permutation()),
            ",")

    );


    auto fmap = wmtk::dart::utils::get_simplex_involution(
        sd.simplex_type(),
        source,
        sd2.simplex_type(),
        target);


    auto [fmap_permutation, fmap_simplex] =
        wmtk::dart::utils::local_simplex_decomposition(sd2, sd.simplex_type(), fmap.permutation());

    spdlog::error(
        "fmap was decomposed to {} from supdart {} with action {} (global was {}). manual "
        "reconstruction is {}",
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd2.simplex_type(), fmap.permutation()),
            ","),
        fmt::join(dart::utils::get_local_vertex_permutation(sd2.simplex_type(), fmap_simplex), ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(sd.simplex_type(), fmap_permutation),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd2.simplex_type(),
                sd.convert(fmap_permutation, sd2)),
            ","),
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd2.simplex_type(),
                sd2.product(upper_simplex, sd.convert(fmap_permutation, sd2))),
            ",")

    );
    REQUIRE(fmap_simplex == upper_simplex);

    // int8_t target_local_permutation = sd2.product(target.permutation(),
    // sd2.inverse(map_simplex));

    // int8_t lower_target = sd2.convert(target_local_permutation, sd);


    // auto fmap2 = wmtk::dart::utils::get_simplex_involution(
    //     sd.simplex_type(),
    //     new_source,
    //     sd2.simplex_type(),
    //     new_target);

    // spdlog::info(
    //     "Maps we recieved were {} -> {}",
    //     fmt::join(
    //         dart::utils::get_local_vertex_permutation(sd2.simplex_type(), fmap.permutation()),
    //         ","),
    //     fmt::join(
    //         dart::utils::get_local_vertex_permutation(sd2.simplex_type(), fmap2.permutation()),
    //         ",")
    //);

    // REQUIRE((fmap == fmap2));


    auto mapped_target = wmtk::dart::utils::apply_simplex_involution(
        sd.simplex_type(),
        sd2.simplex_type(),
        fmap,
        source);

    spdlog::info(
        "Mapping made the target {}",
        fmt::join(
            dart::utils::get_local_vertex_permutation(
                sd2.simplex_type(),
                mapped_target.permutation()),
            ","));

    // just making sure the global id part works first
    CHECK((mapped_target.global_id() == target.global_id()));
    CHECK((mapped_target == target));

    if (sd.simplex_type() != sd2.simplex_type()) {
        auto fmap_reversed = wmtk::dart::utils::get_simplex_involution(
            sd2.simplex_type(),
            target,
            sd.simplex_type(),
            source);
        REQUIRE((fmap == fmap_reversed));

        auto mapped_source = wmtk::dart::utils::apply_simplex_involution(
            sd2.simplex_type(),
            sd.simplex_type(),
            fmap,
            target);
        CHECK((source == mapped_source));

        auto mapped_new_source = wmtk::dart::utils::apply_simplex_involution(
            sd2.simplex_type(),
            sd.simplex_type(),
            fmap,
            new_target);
        CHECK((new_source == mapped_new_source));
    }
}


TEST_CASE("dart_map_1d_same_dim_map", "[dart]")
{
    {
        constexpr static PrimitiveType pt = PrimitiveType::Edge;
        const auto& sd = SimplexDart::get_singleton(pt);


        for (PrimitiveType pt2 : wmtk::utils::primitive_above(pt)) {
            const auto& sd2 = SimplexDart::get_singleton(pt2);
            for (const auto& s : D1) {
                // pt2 is higher htan pt
                //
                int8_t involution = wmtk::dart::utils::get_simplex_involution(
                    pt2,
                    sd2.identity(),
                    pt,
                    s.permutation());
                CHECK(s.permutation() == sd2.convert(involution, sd));
                // CHECK(
                //     s.permutation() ==
                //     );
            }
        }
    }
}
TEST_CASE("dart_map_2d_same_dim_map", "[dart]")
{
    {
        constexpr static PrimitiveType pt = PrimitiveType::Triangle;
        const auto& sd = SimplexDart::get_singleton(pt);


        for (PrimitiveType pt2 : wmtk::utils::primitive_above(pt)) {
            const auto& sd2 = SimplexDart::get_singleton(pt2);
            for (const auto& s : D2) {
                // pt2 is higher htan pt
                //
                int8_t involution = wmtk::dart::utils::get_simplex_involution(
                    pt2,
                    sd2.identity(),
                    pt,
                    s.permutation());
                if (pt2 == pt) {
                    CHECK(s.permutation() == sd2.convert(involution, sd));
                } else {
                    CHECK(s.permutation() == sd2.convert(sd2.inverse(involution), sd));
                }
                // CHECK(
                //     s.permutation() ==
                //     );
            }
        }
    }
}

TEST_CASE("dart_map_1d_1d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Edge;
    const auto& sd = SimplexDart::get_singleton(pt);
    const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Edge);
    for (const auto& s : D1) {
        for (const auto& a : D1) {
            int8_t lower_action = a.permutation();
            for (const auto& t : D1) {
                dart_map_checker(sd, s, sd2, t, lower_action, D1);
            }
        }
    }
}
TEST_CASE("dart_map_1d_2d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Edge;
    const auto& sd = SimplexDart::get_singleton(pt);
    const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Triangle);
    for (const auto& s : D1) {
        for (const auto& a : D1) {
            int8_t lower_action = a.permutation();

            for (const auto& t : D2) {
                dart_map_checker(sd, s, sd2, t, lower_action, D1);
            }
        }
    }
}
TEST_CASE("dart_map_1d_3d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Edge;
    const auto& sd = SimplexDart::get_singleton(pt);
    const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
    for (const auto& s : D1) {
        for (const auto& a : D1) {
            int8_t lower_action = a.permutation();

            for (const auto& t : D3) {
                dart_map_checker(sd, s, sd2, t, lower_action, D1);
            }
        }
    }
}

TEST_CASE("dart_map_2d_2d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Triangle;
    const auto& sd = SimplexDart::get_singleton(pt);
    for (const auto& s : D2) {
        for (const auto& a : D2) {
            int8_t lower_action = a.permutation();

            // for (const auto& t : {d021}) {
            for (const auto& t : D2) {
                dart_map_checker(sd, s, sd, t, lower_action, D2);
                std::cout << std::endl;
            }
        }
    }
}
TEST_CASE("dart_map_2d_3d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Triangle;
    const auto& sd = SimplexDart::get_singleton(pt);
    const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
    // auto s = d210;
    // auto a = d210;
    // auto t = d2013;

    //{
    //    {
    //        {
    for (const auto& s : D2) {
        for (const auto& a : D2) {
            int8_t lower_action = a.permutation();

            for (const auto& t : D3) {
                dart_map_checker(sd, s, sd2, t, lower_action, D2);
            }
        }
    }
}
TEST_CASE("dart_map_3d_3d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
    const auto& sd = SimplexDart::get_singleton(pt);
    const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
    for (const auto& s : D3) {
        for (const auto& a : D3) {
            int8_t lower_action = a.permutation();

            for (const auto& t : D3) {
                dart_map_checker(sd, s, sd2, t, lower_action, D3);
            }
        }
    }
}

TEST_CASE("dart_map_1d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Edge;
    const auto& sd = SimplexDart::get_singleton(pt);
    for (const auto& s : D1) {
        for (const auto& a : D1) {
            int8_t lower_action = a.permutation();
            for (const auto& t : D1) {
                const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Edge);

                dart_map_checker(sd, s, sd2, t, lower_action, D1);
            }

            for (const auto& t : D2) {
                const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Triangle);
                dart_map_checker(sd, s, sd2, t, lower_action, D1);
            }


            for (const auto& t : D3) {
                const auto& sd2 = SimplexDart::get_singleton(PrimitiveType::Tetrahedron);
                dart_map_checker(sd, s, sd2, t, lower_action, D1);
            }
        }
    }
}
TEST_CASE("dart_map_2d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Triangle;
    const auto& sd = SimplexDart::get_singleton(pt);

    const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
    const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);

    const Dart a = d012;
    const Dart b = d021;
    const Dart c = d102;
    const Dart d = d120;
    const Dart e = d201;
    const Dart f = d210;


    // checker(sd, a, PrimitiveType::Vertex, {a, b});
    // checker(sd, c, PrimitiveType::Vertex, {c, d});
    // checker(sd, e, PrimitiveType::Vertex, {e, f});

    // checker(sd, a, PrimitiveType::Edge, {a, c});
    // checker(sd, b, PrimitiveType::Edge, {b, e});
    // checker(sd, d, PrimitiveType::Edge, {d, f});

    // checker(sd, a, PrimitiveType::Triangle, {a, b, c, d, e, f});
}
TEST_CASE("dart_map_3d", "[dart]")
{
    constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
    const auto& sd = SimplexDart::get_singleton(pt);

    const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
    const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);
    const int8_t SF = sd.permutation_index_from_primitive_switch(PrimitiveType::Triangle);


    // checker(sd, d0123, PrimitiveType::Vertex, {d0123, d0132, d0213, d0231, d0312,
    // d0321}); checker(sd, d1023, PrimitiveType::Vertex, {d1023, d1032, d1203, d1230,
    // d1302, d1320}); checker(sd, d2013, PrimitiveType::Vertex, {d2013, d2031, d2103,
    // d2130, d2301, d2310}); checker(sd, d3012, PrimitiveType::Vertex, {d3012, d3021,
    // d3102, d3120, d3201, d3210});


    // checker(sd, d0123, PrimitiveType::Edge, {d0123, d0132, d1023, d1032});
    // checker(sd, d0213, PrimitiveType::Edge, {d0213, d0231, d2013, d2031});
    // checker(sd, d0312, PrimitiveType::Edge, {d0312, d0321, d3012, d3021});
    // checker(sd, d1203, PrimitiveType::Edge, {d1203, d1230, d2103, d2130});
    // checker(sd, d1302, PrimitiveType::Edge, {d1302, d1320, d3102, d3120});
    // checker(sd, d2301, PrimitiveType::Edge, {d2301, d2310, d3201, d3210});


    // checker(sd, d1230, PrimitiveType::Triangle, {d1230, d1320, d2130, d2310, d3120,
    // d3210}); checker(sd, d0231, PrimitiveType::Triangle, {d0231, d0321, d2031, d2301,
    // d3021, d3201}); checker(sd, d0132, PrimitiveType::Triangle, {d0132, d0312, d1032,
    // d1302, d3012, d3102}); checker(sd, d0123, PrimitiveType::Triangle, {d0123, d0213,
    // d1023, d1203, d2013, d2103});

    // checker(sd, d0123, PrimitiveType::Tetrahedron, {d1230, d1320, d2130, d2310,
    // d3120, d3210,
    //                                                 d0231, d0321, d2031, d2301,
    //                                                 d3021, d3201, d0132, d0312,
    //                                                 d1032, d1302, d3012, d3102,
    //                                                 d0123, d0213, d1023, d1203,
    //                                                 d2013, d2103});
}
