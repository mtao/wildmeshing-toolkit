
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_canonical_simplex_orientation.hpp>
#include <wmtk/dart/utils/get_canonical_subdart.hpp>
#include <wmtk/dart/utils/get_canonical_subdart_permutation.hpp>
#include <wmtk/dart/utils/get_canonical_supdart.hpp>
#include <wmtk/dart/utils/get_permutation.hpp>
#include <wmtk/dart/utils/permute.hpp>
#include "utils/canonical_darts.hpp"

#include <wmtk/dart/utils/from_local_vertex_permutation.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include <wmtk/utils/primitive_range.hpp>
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

TEST_CASE("dart_canonical_subdart", "[dart]")
{
    auto checker = [&]<PrimitiveType PT, PrimitiveType PT2>(
                       std::integral_constant<PrimitiveType, PT> base_pt,
                       const Dart& canonical,
                       std::integral_constant<PrimitiveType, PT2> pt,
                       const std::vector<Dart>& list) {
        const SimplexDart& sd = SimplexDart::get_singleton(base_pt);
        int8_t pid = canonical.permutation();
        for (const auto& l : list) {
            int8_t canonical_index =
                wmtk::dart::utils::get_canonical_subdart(sd, pt, l.permutation());
            CHECK(int64_t(pid) == int64_t(canonical_index));

            constexpr int8_t base_size = get_primitive_type_id(base_pt) + 1;
            constexpr int8_t size = get_primitive_type_id(pt) + 1;

            // std::array<int8_t, base_size> base =
            dart::utils::get_local_vertex_permutation<get_primitive_type_id(base_pt)>(
                canonical.permutation());
            // std::array<int8_t, size> s =
            dart::utils::get_local_vertex_permutation<get_primitive_type_id(pt)>(l.permutation());


            // int8_t subdart = dart::utils::get_canonical_subdart_permutation(base, s);
            // std::array<int8_t, base_size> subdart_indices =
            //     dart::utils::get_canonical_subdart_permutation_indices(base, s);
        }
    };
    using VT = std::integral_constant<PrimitiveType, PrimitiveType::Vertex>;
    using ET = std::integral_constant<PrimitiveType, PrimitiveType::Edge>;
    using FT = std::integral_constant<PrimitiveType, PrimitiveType::Triangle>;
    using TT = std::integral_constant<PrimitiveType, PrimitiveType::Tetrahedron>;
    {
        constexpr static PrimitiveType pt = PrimitiveType::Edge;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        // const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);

        const Dart a = d01;
        const Dart b = d10;

        checker(ET{}, a, VT{}, {a});
        checker(ET{}, b, VT{}, {b});

        checker(ET{}, a, ET{}, {a, b});
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


        checker(FT{}, a, VT{}, {a, b});
        checker(FT{}, c, VT{}, {c, d});
        checker(FT{}, e, VT{}, {e, f});

        checker(FT{}, a, ET{}, {a});
        checker(FT{}, b, ET{}, {b});
        checker(FT{}, c, ET{}, {c});
        checker(FT{}, d, ET{}, {d});
        checker(FT{}, e, ET{}, {e});
        checker(FT{}, f, ET{}, {f});

        // TODO: this feels wrong
        checker(FT{}, a, FT{}, {a, b, c, d, e, f});
    }
    {
        constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);
        const int8_t SF = sd.permutation_index_from_primitive_switch(PrimitiveType::Triangle);


        checker(TT{}, d0123, VT{}, {d0123, d0132, d0213, d0231, d0312, d0321});
        checker(TT{}, d1023, VT{}, {d1023, d1032, d1203, d1230, d1302, d1320});
        checker(TT{}, d2013, VT{}, {d2013, d2031, d2103, d2130, d2301, d2310});
        checker(TT{}, d3012, VT{}, {d3012, d3021, d3102, d3120, d3201, d3210});


        checker(TT{}, d0123, ET{}, {d0123, d0132});
        checker(TT{}, d1023, ET{}, {d1023, d1032});
        checker(TT{}, d0213, ET{}, {d0213, d0231});
        checker(TT{}, d2013, ET{}, {d2013, d2031});
        checker(TT{}, d0312, ET{}, {d0312, d0321});
        checker(TT{}, d3012, ET{}, {d3012, d3021});
        checker(TT{}, d1203, ET{}, {d1203, d1230});
        checker(TT{}, d2103, ET{}, {d2103, d2130});
        checker(TT{}, d1302, ET{}, {d1302, d1320});
        checker(TT{}, d3102, ET{}, {d3102, d3120});
        checker(TT{}, d2301, ET{}, {d2301, d2310});
        checker(TT{}, d3201, ET{}, {d3201, d3210});


        checker(TT{}, d0123, FT{}, {d0123});
        checker(TT{}, d0213, FT{}, {d0213});
        checker(TT{}, d1023, FT{}, {d1023});
        checker(TT{}, d1203, FT{}, {d1203});
        checker(TT{}, d2013, FT{}, {d2013});
        checker(TT{}, d2103, FT{}, {d2103});
        checker(TT{}, d0132, FT{}, {d0132});
        checker(TT{}, d0231, FT{}, {d0231});
        checker(TT{}, d1032, FT{}, {d1032});
        checker(TT{}, d1230, FT{}, {d1230});
        checker(TT{}, d2031, FT{}, {d2031});
        checker(TT{}, d2130, FT{}, {d2130});
        checker(TT{}, d0312, FT{}, {d0312});
        checker(TT{}, d0321, FT{}, {d0321});
        checker(TT{}, d1302, FT{}, {d1302});
        checker(TT{}, d1320, FT{}, {d1320});
        checker(TT{}, d2301, FT{}, {d2301});
        checker(TT{}, d2310, FT{}, {d2310});
        checker(TT{}, d3012, FT{}, {d3012});
        checker(TT{}, d3021, FT{}, {d3021});
        checker(TT{}, d3102, FT{}, {d3102});
        checker(TT{}, d3120, FT{}, {d3120});
        checker(TT{}, d3201, FT{}, {d3201});
        checker(TT{}, d3210, FT{}, {d3210});
    }
}

TEST_CASE("dart_canonical_supdart", "[dart]")
{
    auto checker = [&](const SimplexDart& sd,
                       const Dart& canonical,
                       PrimitiveType pt,
                       const std::vector<Dart>& list) {
        int8_t pid = canonical.permutation();
        for (const auto& l : list) {
            int8_t canonical_index =
                wmtk::dart::utils::get_canonical_supdart(sd, pt, l.permutation());
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


        checker(sd, a, PrimitiveType::Vertex, {a});
        checker(sd, b, PrimitiveType::Vertex, {b});
        checker(sd, c, PrimitiveType::Vertex, {c});
        checker(sd, d, PrimitiveType::Vertex, {d});
        checker(sd, e, PrimitiveType::Vertex, {e});
        checker(sd, f, PrimitiveType::Vertex, {f});

        checker(sd, a, PrimitiveType::Edge, {a, c});
        checker(sd, b, PrimitiveType::Edge, {b, e});
        checker(sd, d, PrimitiveType::Edge, {d, f});


        // TODO: this feels wrong
        checker(sd, a, PrimitiveType::Triangle, {a, b, c, d, e, f});
    }
    {
        constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);
        const int8_t SF = sd.permutation_index_from_primitive_switch(PrimitiveType::Triangle);


        checker(sd, d0123, PrimitiveType::Vertex, {d0123});
        checker(sd, d0213, PrimitiveType::Vertex, {d0213});
        checker(sd, d1023, PrimitiveType::Vertex, {d1023});
        checker(sd, d1203, PrimitiveType::Vertex, {d1203});
        checker(sd, d2013, PrimitiveType::Vertex, {d2013});
        checker(sd, d2103, PrimitiveType::Vertex, {d2103});
        checker(sd, d0132, PrimitiveType::Vertex, {d0132});
        checker(sd, d0231, PrimitiveType::Vertex, {d0231});
        checker(sd, d1032, PrimitiveType::Vertex, {d1032});
        checker(sd, d1230, PrimitiveType::Vertex, {d1230});
        checker(sd, d2031, PrimitiveType::Vertex, {d2031});
        checker(sd, d2130, PrimitiveType::Vertex, {d2130});
        checker(sd, d0312, PrimitiveType::Vertex, {d0312});
        checker(sd, d0321, PrimitiveType::Vertex, {d0321});
        checker(sd, d1302, PrimitiveType::Vertex, {d1302});
        checker(sd, d1320, PrimitiveType::Vertex, {d1320});
        checker(sd, d2301, PrimitiveType::Vertex, {d2301});
        checker(sd, d2310, PrimitiveType::Vertex, {d2310});
        checker(sd, d3012, PrimitiveType::Vertex, {d3012});
        checker(sd, d3021, PrimitiveType::Vertex, {d3021});
        checker(sd, d3102, PrimitiveType::Vertex, {d3102});
        checker(sd, d3120, PrimitiveType::Vertex, {d3120});
        checker(sd, d3201, PrimitiveType::Vertex, {d3201});
        checker(sd, d3210, PrimitiveType::Vertex, {d3210});


        checker(sd, d1230, PrimitiveType::Triangle, {d1230, d2130, d2310, d1320, d2310, d3210});
        checker(sd, d0231, PrimitiveType::Triangle, {d0231, d2031, d2301, d0321, d2301, d3201});
        checker(sd, d0132, PrimitiveType::Triangle, {d0132, d1032, d1302, d0312, d1302, d3102});
        checker(sd, d0123, PrimitiveType::Triangle, {d0123, d1023, d1203, d0213, d1203, d2103});


        checker(sd, d2301, PrimitiveType::Edge, {d2301, d3201});
        checker(sd, d2310, PrimitiveType::Edge, {d2310, d3210});
        checker(sd, d1302, PrimitiveType::Edge, {d1302, d3102});
        checker(sd, d1320, PrimitiveType::Edge, {d1320, d3120});
        checker(sd, d1203, PrimitiveType::Edge, {d1203, d2103});
        checker(sd, d1230, PrimitiveType::Edge, {d1230, d2130});
        checker(sd, d0312, PrimitiveType::Edge, {d0312, d3012});
        checker(sd, d0321, PrimitiveType::Edge, {d0321, d3021});
        checker(sd, d0213, PrimitiveType::Edge, {d0213, d2013});
        checker(sd, d0231, PrimitiveType::Edge, {d0231, d2031});
        checker(sd, d0123, PrimitiveType::Edge, {d0123, d1023});
        checker(sd, d0132, PrimitiveType::Edge, {d0132, d1032});
    }
}

TEST_CASE("dart_subdart_permutation", "[dart]")
{
    // checks that the permutation provided is the permutation of the dart with respect to its
    // canonical supdart input is the permutation that
    auto checker = [&](const SimplexDart& sd,
                       const Dart& source,
                       PrimitiveType pt,
                       const int8_t& permutation) {
        // int8_t forward = sd.product(source.permutation(), permutation);
        int8_t canonical_supdart =
            wmtk::dart::utils::get_canonical_supdart(sd, pt, source.permutation());
        int8_t forward = sd.act(canonical_supdart, permutation);
        std::cout << "Source: "
                  << wmtk::dart::utils::get_local_vertex_permutation(
                         sd.simplex_type(),
                         source.permutation())
                         .transpose()
                  << std::endl;
        std::cout << "Permutation: "
                  << wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(), permutation)
                         .transpose()
                  << std::endl;
        std::cout << "Canonical: "
                  << wmtk::dart::utils::get_local_vertex_permutation(
                         sd.simplex_type(),
                         canonical_supdart)
                         .transpose()
                  << std::endl;
        std::cout << "Product: "
                  << wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(), forward)
                         .transpose()
                  << std::endl;
        std::cout << "reconstruct: "
                  << wmtk::dart::utils::get_local_vertex_permutation(
                         sd.simplex_type(),
                         sd.act(source.permutation(), canonical_supdart))
                         .transpose()
                  << std::endl;
        CHECK(source.permutation() == forward);
        // CHECK(forward ==


        // int8_t canonical =
        //     wmtk::dart::utils::get_canonical_simplex_orientation(sd, pt, source.permutation());
        // std::cout << "Canonical: " <<
        // wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(), canonical).transpose()
        // << std::endl; std::cout << "Permutation: " <<
        // wmtk::dart::utils::get_local_vertex_permutation(sd.simplex_type(),
        // permutation).transpose() << std::endl; spdlog::info("{} {}", canonical, permutation);
        // CHECK(permutation == canonical);
    };
    {
        constexpr static PrimitiveType pt = PrimitiveType::Edge;
        const auto& sd = SimplexDart::get_singleton(pt);

        // const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);


        // doing on vertex is identity permutation
        checker(sd, d01, PrimitiveType::Vertex, d01.permutation());
        checker(sd, d10, PrimitiveType::Vertex, d01.permutation());

        // sorta silly as it does nothing
        checker(sd, d10, PrimitiveType::Edge, d10.permutation());
        checker(sd, d01, PrimitiveType::Edge, d01.permutation());
    }
    {
        spdlog::info("Tri");
        constexpr static PrimitiveType pt = PrimitiveType::Triangle;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);


        for (const auto& d : D2) {
            checker(sd, d, PrimitiveType::Vertex, d012.permutation());
        }

        spdlog::info("a");
        checker(sd, d012, PrimitiveType::Edge, d012.permutation());
        spdlog::info("b");
        checker(sd, d021, PrimitiveType::Edge, d012.permutation());
        spdlog::info("c");
        checker(sd, d102, PrimitiveType::Edge, d102.permutation());
        spdlog::info("d");
        checker(sd, d120, PrimitiveType::Edge, d012.permutation());
        spdlog::info("e");
        checker(sd, d201, PrimitiveType::Edge, d102.permutation());
        spdlog::info("f");
        checker(sd, d210, PrimitiveType::Edge, d102.permutation());
        return;

        for (const auto& d : D2) {
            checker(sd, d, PrimitiveType::Triangle, d.permutation());
        }
    }
    {
        constexpr static PrimitiveType pt = PrimitiveType::Tetrahedron;
        const auto& sd = SimplexDart::get_singleton(pt);

        const int8_t SV = sd.permutation_index_from_primitive_switch(PrimitiveType::Vertex);
        const int8_t SE = sd.permutation_index_from_primitive_switch(PrimitiveType::Edge);
        const int8_t SF = sd.permutation_index_from_primitive_switch(PrimitiveType::Triangle);

        for (const auto& d : D3) {
            checker(sd, d, PrimitiveType::Vertex, d0123.permutation());
        }

        checker(sd, d0123, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d0213, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d1023, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d1203, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d2013, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d2103, PrimitiveType::Edge, d1023.permutation());

        checker(sd, d0213, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d0123, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d2013, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d2103, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d1023, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d1203, PrimitiveType::Edge, d0123.permutation());

        checker(sd, d0312, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d0132, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d3012, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d3102, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d1032, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d1302, PrimitiveType::Edge, d0123.permutation());

        checker(sd, d0321, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d0231, PrimitiveType::Edge, d0123.permutation());
        checker(sd, d3021, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d3201, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d2031, PrimitiveType::Edge, d1023.permutation());
        checker(sd, d2301, PrimitiveType::Edge, d0123.permutation());

        //

        checker(sd, d0123, PrimitiveType::Triangle, d0123.permutation());
        checker(sd, d0213, PrimitiveType::Triangle, d0213.permutation());
        checker(sd, d1023, PrimitiveType::Triangle, d1023.permutation());
        checker(sd, d1203, PrimitiveType::Triangle, d1203.permutation());
        checker(sd, d2013, PrimitiveType::Triangle, d2013.permutation());
        checker(sd, d2103, PrimitiveType::Triangle, d2103.permutation());

        checker(sd, d0132, PrimitiveType::Triangle, d0123.permutation());
        checker(sd, d0312, PrimitiveType::Triangle, d0213.permutation());
        checker(sd, d1032, PrimitiveType::Triangle, d1023.permutation());
        checker(sd, d1302, PrimitiveType::Triangle, d1203.permutation());
        checker(sd, d3012, PrimitiveType::Triangle, d2013.permutation());
        checker(sd, d3102, PrimitiveType::Triangle, d2103.permutation());

        checker(sd, d0231, PrimitiveType::Triangle, d0123.permutation());
        checker(sd, d0321, PrimitiveType::Triangle, d0213.permutation());
        checker(sd, d2031, PrimitiveType::Triangle, d1023.permutation());
        checker(sd, d2301, PrimitiveType::Triangle, d1203.permutation());
        checker(sd, d3021, PrimitiveType::Triangle, d2013.permutation());
        checker(sd, d3201, PrimitiveType::Triangle, d2103.permutation());

        checker(sd, d1230, PrimitiveType::Triangle, d0123.permutation());
        checker(sd, d1320, PrimitiveType::Triangle, d0213.permutation());
        checker(sd, d2130, PrimitiveType::Triangle, d1023.permutation());
        checker(sd, d2310, PrimitiveType::Triangle, d1203.permutation());
        checker(sd, d3120, PrimitiveType::Triangle, d2013.permutation());
        checker(sd, d3210, PrimitiveType::Triangle, d2103.permutation());
    }
}


TEST_CASE("permute_operator", "[permutation]")
{
    auto run = []<size_t N>(const std::array<int64_t, N>& p) {
        const PrimitiveType pt = get_primitive_type_from_id(int8_t(N - 1));
        const auto& sd = dart::SimplexDart::get_singleton(pt);
        for (int8_t perm_index = 0; perm_index < sd.size(); ++perm_index) {
            auto perm = dart::utils::get_local_vertex_permutation(pt, perm_index);

            std::array<int64_t, N> o;
            for (size_t j = 0; j < N; ++j) {
                o[j] = p[perm[j]];
            }
            CHECK(dart::utils::permute(p, perm_index) == o);

            spdlog::warn("Applied perm: {}", fmt::join(perm, ","));
            CHECK(dart::utils::get_permutation(p, o) == perm_index);

            for (int8_t perm_index2 = 0; perm_index2 < sd.size(); ++perm_index2) {
                auto o2 = dart::utils::permute(o, perm_index2);

                int8_t act = sd.act(perm_index, perm_index2);

                auto perm2 = dart::utils::get_local_vertex_permutation(pt, perm_index2);
                spdlog::warn("Applied perm2: {}", fmt::join(perm2, ","));
                CHECK(dart::utils::get_permutation(o, o2) == perm_index2);


                CHECK(act == sd.product(perm_index, perm_index2));


                auto op = dart::utils::permute(p, act);


                CHECK(op == o2);
            }
        }
    };
    run(std::array<int64_t, 3>{{5, 6, 7}});
    // run(std::array<int64_t, 3>{{5, 7, 6}});
    // run(std::array<int64_t, 4>{{5, 2, 7, 6}});
}


namespace {
template <int8_t Dim>
int8_t swap(int8_t a, int8_t b)
{
    std::array<int8_t, Dim + 1> indices;
    for (int8_t j = 0; j <= Dim; ++j) {
        if (j == a) {
            indices[j] = b;
        } else if (j == b) {
            indices[j] = a;
        } else {
            indices[j] = j;
        }
    }
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(Dim);
    return dart::utils::from_local_vertex_permutation(indices);
}

TEST_CASE("swap_edge_permutation", "[permutation]")
{
    /*
    auto run = []<int8_t N>(std::integral_constant<int8_t, N>) {
        const PrimitiveType pt = get_primitive_type_from_id(int8_t(N - 1));
        const auto& sd = dart::SimplexDart::get_singleton(pt);
        for (int8_t perm_index = 0; perm_index < sd.size(); ++perm_index) {
            auto perm = dart::utils::get_local_vertex_permutation(pt, perm_index);
        }
    };


    run(std::integral_constant<int8_t, 1>{});
    run(std::integral_constant<int8_t, 2>{});
    run(std::integral_constant<int8_t, 3>{});
    */
    {
        const PrimitiveType pt = PrimitiveType::Triangle;
        const auto& sd = dart::SimplexDart::get_singleton(pt);
        int8_t s01 = swap<2>(0, 1);
        int8_t s12 = swap<2>(1, 2);
        int8_t s02 = swap<2>(2, 0);
        spdlog::info("s01 {}", dart::utils::get_local_vertex_permutation(pt, s01));
        spdlog::info("s12 {}", dart::utils::get_local_vertex_permutation(pt, s12));
        spdlog::info("s02 {}", dart::utils::get_local_vertex_permutation(pt, s02));
        auto op = [&](int8_t p) {
            int8_t pinv = sd.inverse(p);
            // int8_t r = sd.product({(p), d102.permutation(), sd.inverse(p)});
            int8_t tmp = sd.product(p, d102.permutation());
            int8_t r = sd.product(tmp, pinv);
            // int8_t r = sd.product({sd.inverse(p), d102.permutation(), p});
            spdlog::info(
                "{} => {} => {}",
                dart::utils::get_local_vertex_permutation(pt, tmp),
                dart::utils::get_local_vertex_permutation(pt, p),
                dart::utils::get_local_vertex_permutation(pt, r));
            return r;
        };


        op(d012.permutation());
        op(d102.permutation());
        op(d021.permutation());


        // spdlog::info("s01 {}", dart::utils::get_local_vertex_permutation(pt, s012));
    }
}


} // namespace
