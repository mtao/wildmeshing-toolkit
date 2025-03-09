
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_canonical_simplex_orientation.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "utils/canonical_darts.hpp"
using namespace wmtk;
using namespace wmtk::dart;
using namespace wmtk::tests::dart::utils;

TEST_CASE("tuple_autogen_index_dart_group_structure", "[tuple][dart]")
{
    // when other meshes are available add them here
    for (PrimitiveType mesh_type :
         {PrimitiveType::Edge, PrimitiveType::Triangle, PrimitiveType::Tetrahedron}) {
        const auto& sd = dart::SimplexDart::get_singleton(mesh_type);

        for (PrimitiveType pt : wmtk::utils::primitive_below(mesh_type)) {
            const int8_t index_switch = sd.primitive_as_index(pt);
            CHECK(sd.identity() == sd.product(index_switch, index_switch));
            CHECK(index_switch == sd.inverse(index_switch));
        }
        for (int8_t index = 0; index < sd.size(); ++index) {
            const int8_t inv = sd.inverse(index);
            CHECK(sd.product(index, inv) == sd.identity());
            CHECK(sd.product(inv, index) == sd.identity());
            // for (int8_t index2 = 0; index2 < sd.size(); ++index2) {
            //     const int8_t inv2 = sd.inverse(index2);
            //     const int8_t p = sd.product(index, index2);
            //     const int8_t pi = sd.product(inv2, inv);
            //     CHECK(pi == sd.inverse(p));
            //     CHECK(sd.product(p, pi) == sd.identity());
            //     CHECK(sd.product(pi, p) == sd.identity());
            // }
        }
    }
}

TEST_CASE("dart_from_vertex_permutation", "[dart]")
{
    {
        constexpr static PrimitiveType pt = PrimitiveType::Edge;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        // const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);

        const Dart a = d01;
        const Dart b = d10;

        CHECK((b == sd.act(a, SV)));
        CHECK((a == sd.act(b, SV)));
    }
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

        CHECK((a == sd.act(c, SV)));
        CHECK((b == sd.act(e, SV)));
        CHECK((c == sd.act(a, SV)));
        CHECK((d == sd.act(f, SV)));
        CHECK((e == sd.act(b, SV)));
        CHECK((f == sd.act(d, SV)));


        CHECK((a == sd.act(b, SE)));
        CHECK((b == sd.act(a, SE)));
        CHECK((c == sd.act(d, SE)));
        CHECK((d == sd.act(c, SE)));
        CHECK((e == sd.act(f, SE)));
        CHECK((f == sd.act(e, SE)));
    }
    {
        constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);
        const int8_t SF = sd.permutation_index_from_primitive_switch(PrimitiveType::Triangle);


        CHECK((d0123 == sd.act(d1023, SV)));
        CHECK((d0213 == sd.act(d2013, SV)));
        CHECK((d1023 == sd.act(d0123, SV)));
        CHECK((d1203 == sd.act(d2103, SV)));
        CHECK((d2013 == sd.act(d0213, SV)));
        CHECK((d2103 == sd.act(d1203, SV)));

        CHECK((d0132 == sd.act(d1032, SV)));
        CHECK((d0231 == sd.act(d2031, SV)));
        CHECK((d1032 == sd.act(d0132, SV)));
        CHECK((d1230 == sd.act(d2130, SV)));
        CHECK((d2031 == sd.act(d0231, SV)));
        CHECK((d2130 == sd.act(d1230, SV)));

        CHECK((d0312 == sd.act(d3012, SV)));
        CHECK((d0321 == sd.act(d3021, SV)));
        CHECK((d1302 == sd.act(d3102, SV)));
        CHECK((d1320 == sd.act(d3120, SV)));
        CHECK((d2301 == sd.act(d3201, SV)));
        CHECK((d2310 == sd.act(d3210, SV)));

        CHECK((d3012 == sd.act(d0312, SV)));
        CHECK((d3021 == sd.act(d0321, SV)));
        CHECK((d3102 == sd.act(d1302, SV)));
        CHECK((d3120 == sd.act(d1320, SV)));
        CHECK((d3201 == sd.act(d2301, SV)));
        CHECK((d3210 == sd.act(d2310, SV)));

        CHECK((d0123 == sd.act(d0213, SE)));
        CHECK((d0213 == sd.act(d0123, SE)));
        CHECK((d1023 == sd.act(d1203, SE)));
        CHECK((d1203 == sd.act(d1023, SE)));
        CHECK((d2013 == sd.act(d2103, SE)));
        CHECK((d2103 == sd.act(d2013, SE)));

        CHECK((d0132 == sd.act(d0312, SE)));
        CHECK((d0231 == sd.act(d0321, SE)));
        CHECK((d1032 == sd.act(d1302, SE)));
        CHECK((d1230 == sd.act(d1320, SE)));
        CHECK((d2031 == sd.act(d2301, SE)));
        CHECK((d2130 == sd.act(d2310, SE)));

        CHECK((d0312 == sd.act(d0132, SE)));
        CHECK((d0321 == sd.act(d0231, SE)));
        CHECK((d1302 == sd.act(d1032, SE)));
        CHECK((d1320 == sd.act(d1230, SE)));
        CHECK((d2301 == sd.act(d2031, SE)));
        CHECK((d2310 == sd.act(d2130, SE)));

        CHECK((d3012 == sd.act(d3102, SE)));
        CHECK((d3021 == sd.act(d3201, SE)));
        CHECK((d3102 == sd.act(d3012, SE)));
        CHECK((d3120 == sd.act(d3210, SE)));
        CHECK((d3201 == sd.act(d3021, SE)));
        CHECK((d3210 == sd.act(d3120, SE)));

        CHECK((d0123 == sd.act(d0132, SF)));
        CHECK((d0213 == sd.act(d0231, SF)));
        CHECK((d1023 == sd.act(d1032, SF)));
        CHECK((d1203 == sd.act(d1230, SF)));
        CHECK((d2013 == sd.act(d2031, SF)));
        CHECK((d2103 == sd.act(d2130, SF)));

        CHECK((d0132 == sd.act(d0123, SF)));
        CHECK((d0231 == sd.act(d0213, SF)));
        CHECK((d1032 == sd.act(d1023, SF)));
        CHECK((d1230 == sd.act(d1203, SF)));
        CHECK((d2031 == sd.act(d2013, SF)));
        CHECK((d2130 == sd.act(d2103, SF)));

        CHECK((d0312 == sd.act(d0321, SF)));
        CHECK((d0321 == sd.act(d0312, SF)));
        CHECK((d1302 == sd.act(d1320, SF)));
        CHECK((d1320 == sd.act(d1302, SF)));
        CHECK((d2301 == sd.act(d2310, SF)));
        CHECK((d2310 == sd.act(d2301, SF)));

        CHECK((d3012 == sd.act(d3021, SF)));
        CHECK((d3021 == sd.act(d3012, SF)));
        CHECK((d3102 == sd.act(d3120, SF)));
        CHECK((d3120 == sd.act(d3102, SF)));
        CHECK((d3201 == sd.act(d3210, SF)));
        CHECK((d3210 == sd.act(d3201, SF)));
    }
}
TEST_CASE("dart_canonical_orientation", "[dart]")
{
    auto checker = [&](const SimplexDart& sd,
                       const Dart& canonical,
                       PrimitiveType pt,
                       const std::vector<Dart>& list) {
        int8_t pid = canonical.permutation();
        for (const auto& l : list) {
            int8_t canonical_index =
                wmtk::dart::utils::get_canonical_simplex_orientation(sd, pt, l.permutation());
            CHECK(int64_t(pid) == int64_t(canonical_index));
        }
    };
    {
        constexpr static PrimitiveType pt = PrimitiveType::Edge;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        // const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);

        const Dart a = d01;
        const Dart b = d10;

        checker(sd, a, PrimitiveType::Vertex, {a});
        checker(sd, b, PrimitiveType::Vertex, {b});

        checker(sd, a, PrimitiveType::Edge, {a, b});
    }
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


        checker(sd, a, PrimitiveType::Vertex, {a, b});
        checker(sd, c, PrimitiveType::Vertex, {c, d});
        checker(sd, e, PrimitiveType::Vertex, {e, f});

        checker(sd, a, PrimitiveType::Edge, {a, c});
        checker(sd, b, PrimitiveType::Edge, {b, e});
        checker(sd, d, PrimitiveType::Edge, {d, f});

        checker(sd, a, PrimitiveType::Triangle, {a, b, c, d, e, f});
    }
    {
        constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);
        const int8_t SF = sd.permutation_index_from_primitive_switch(PrimitiveType::Triangle);


        checker(sd, d0123, PrimitiveType::Vertex, {d0123, d0132, d0213, d0231, d0312, d0321});
        checker(sd, d1023, PrimitiveType::Vertex, {d1023, d1032, d1203, d1230, d1302, d1320});
        checker(sd, d2013, PrimitiveType::Vertex, {d2013, d2031, d2103, d2130, d2301, d2310});
        checker(sd, d3012, PrimitiveType::Vertex, {d3012, d3021, d3102, d3120, d3201, d3210});


        checker(sd, d0123, PrimitiveType::Edge, {d0123, d0132, d1023, d1032});
        checker(sd, d0213, PrimitiveType::Edge, {d0213, d0231, d2013, d2031});
        checker(sd, d0312, PrimitiveType::Edge, {d0312, d0321, d3012, d3021});
        checker(sd, d1203, PrimitiveType::Edge, {d1203, d1230, d2103, d2130});
        checker(sd, d1302, PrimitiveType::Edge, {d1302, d1320, d3102, d3120});
        checker(sd, d2301, PrimitiveType::Edge, {d2301, d2310, d3201, d3210});


        checker(sd, d1230, PrimitiveType::Triangle, {d1230, d1320, d2130, d2310, d3120, d3210});
        checker(sd, d0231, PrimitiveType::Triangle, {d0231, d0321, d2031, d2301, d3021, d3201});
        checker(sd, d0132, PrimitiveType::Triangle, {d0132, d0312, d1032, d1302, d3012, d3102});
        checker(sd, d0123, PrimitiveType::Triangle, {d0123, d0213, d1023, d1203, d2013, d2103});

        checker(sd, d0123, PrimitiveType::Tetrahedron, {d1230, d1320, d2130, d2310, d3120, d3210,
                                                        d0231, d0321, d2031, d2301, d3021, d3201,
                                                        d0132, d0312, d1032, d1302, d3012, d3102,
                                                        d0123, d0213, d1023, d1203, d2013, d2103});
    }
}
