#include <spdlog/spdlog.h>
#include <algorithm>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include <wmtk/utils/primitive_range_iter.hpp>

using namespace wmtk;
TEST_CASE("primitive_range", "[primitive]")
{
    for (PrimitiveType pt :
         {PrimitiveType::Vertex,
          PrimitiveType::Edge,
          PrimitiveType::Triangle,
          PrimitiveType::Tetrahedron}) {
        {
            auto a = wmtk::utils::primitive_range(pt, PrimitiveType::Tetrahedron);
            auto b = wmtk::utils::primitive_above(pt);
            CHECK(a == b);
        }
        {
            auto a = wmtk::utils::primitive_range(PrimitiveType::Vertex, pt);
            auto b = wmtk::utils::primitive_below(pt);
            std::reverse(b.begin(), b.end());
            CHECK(a == b);
        }
    }
    // 1,1
    // 1,2
    // 2,2
    {
        auto a = wmtk::utils::primitive_range(PrimitiveType::Edge, PrimitiveType::Edge);
        std::vector<PrimitiveType> b{PrimitiveType::Edge};
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_range(PrimitiveType::Triangle, PrimitiveType::Triangle);
        std::vector<PrimitiveType> b{PrimitiveType::Triangle};
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_range(PrimitiveType::Edge, PrimitiveType::Triangle);
        std::vector<PrimitiveType> b{PrimitiveType::Edge, PrimitiveType::Triangle};
        CHECK(a == b);
    }
}
TEST_CASE("primitive_above", "[primitive]")
{
    {
        auto a = wmtk::utils::primitive_above(PrimitiveType::Tetrahedron);
        std::vector<PrimitiveType> b{PrimitiveType::Tetrahedron};
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_above(PrimitiveType::Triangle);
        std::vector<PrimitiveType> b{PrimitiveType::Triangle, PrimitiveType::Tetrahedron};
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_above(PrimitiveType::Edge);
        std::vector<PrimitiveType> b{
            PrimitiveType::Edge,
            PrimitiveType::Triangle,
            PrimitiveType::Tetrahedron,
        };
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_above(PrimitiveType::Vertex);
        std::vector<PrimitiveType> b{
            PrimitiveType::Vertex,
            PrimitiveType::Edge,
            PrimitiveType::Triangle,
            PrimitiveType::Tetrahedron};
        CHECK(a == b);
    }
}
TEST_CASE("primitive_below", "[primitive]")
{
    {
        auto a = wmtk::utils::primitive_below(PrimitiveType::Tetrahedron);
        std::vector<PrimitiveType> b{
            PrimitiveType::Tetrahedron,
            PrimitiveType::Triangle,
            PrimitiveType::Edge,
            PrimitiveType::Vertex,
        };
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_below(PrimitiveType::Triangle);
        std::vector<PrimitiveType> b{
            PrimitiveType::Triangle,
            PrimitiveType::Edge,
            PrimitiveType::Vertex,
        };
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_below(PrimitiveType::Edge);
        std::vector<PrimitiveType> b{
            PrimitiveType::Edge,
            PrimitiveType::Vertex,
        };
        CHECK(a == b);
    }
    {
        auto a = wmtk::utils::primitive_below(PrimitiveType::Vertex);
        std::vector<PrimitiveType> b{PrimitiveType::Vertex};
        CHECK(a == b);
    }
}


namespace {
template <PrimitiveType pt, bool l2u>
void test_below()
{
    auto below = wmtk::utils::primitive_below(pt, l2u);
    auto below_it = wmtk::utils::primitive_below_iter(pt, l2u);

    std::vector<PrimitiveType> below_it_vec(below_it.begin(), below_it.end());
    CHECK(below == below_it_vec);
}
template <PrimitiveType pt, bool l2u>
void test_above()
{
    auto above = wmtk::utils::primitive_above(pt, l2u);
    auto above_it = wmtk::utils::primitive_above_iter(pt, l2u);

    std::vector<PrimitiveType> above_it_vec(above_it.begin(), above_it.end());
    CHECK(above == above_it_vec);
}
template <PrimitiveType Start, PrimitiveType End>
void test_range()
{
    spdlog::info("{} {}", int8_t(Start), int8_t(End));
    auto range = wmtk::utils::primitive_range(Start, End);
    auto range_it = wmtk::utils::primitive_range_iter(Start, End);

    std::vector<PrimitiveType> range_it_vec(range_it.begin(), range_it.end());
    REQUIRE(range.size() == range_it_vec.size());
    for (size_t j = 0; j < range.size(); ++j) {
        spdlog::warn("{} {}", int8_t(range[j]), int8_t(range_it_vec[j]));
    }

    CHECK(range == range_it_vec);
}
} // namespace

TEST_CASE("primitive_range_iteration", "[primitive]")
{
    test_below<PrimitiveType::Vertex, false>();
    test_below<PrimitiveType::Edge, false>();
    test_below<PrimitiveType::Triangle, false>();
    test_below<PrimitiveType::Tetrahedron, false>();
    test_below<PrimitiveType::Vertex, true>();
    test_below<PrimitiveType::Edge, true>();
    test_below<PrimitiveType::Triangle, true>();
    test_below<PrimitiveType::Tetrahedron, true>();

    test_above<PrimitiveType::Vertex, false>();
    test_above<PrimitiveType::Edge, false>();
    test_above<PrimitiveType::Triangle, false>();
    test_above<PrimitiveType::Tetrahedron, false>();
    test_above<PrimitiveType::Vertex, true>();
    test_above<PrimitiveType::Edge, true>();
    test_above<PrimitiveType::Triangle, true>();
    test_above<PrimitiveType::Tetrahedron, true>();

    test_range<PrimitiveType::Vertex, PrimitiveType::Vertex>();
    test_range<PrimitiveType::Edge, PrimitiveType::Vertex>();
    test_range<PrimitiveType::Triangle, PrimitiveType::Vertex>();
    test_range<PrimitiveType::Tetrahedron, PrimitiveType::Vertex>();

    test_range<PrimitiveType::Vertex, PrimitiveType::Edge>();
    test_range<PrimitiveType::Edge, PrimitiveType::Edge>();
    test_range<PrimitiveType::Triangle, PrimitiveType::Edge>();
    test_range<PrimitiveType::Tetrahedron, PrimitiveType::Edge>();

    test_range<PrimitiveType::Vertex, PrimitiveType::Triangle>();
    test_range<PrimitiveType::Edge, PrimitiveType::Triangle>();
    test_range<PrimitiveType::Triangle, PrimitiveType::Triangle>();
    test_range<PrimitiveType::Tetrahedron, PrimitiveType::Triangle>();

    test_range<PrimitiveType::Vertex, PrimitiveType::Tetrahedron>();
    test_range<PrimitiveType::Edge, PrimitiveType::Tetrahedron>();
    test_range<PrimitiveType::Triangle, PrimitiveType::Tetrahedron>();
    test_range<PrimitiveType::Tetrahedron, PrimitiveType::Tetrahedron>();
}

TEST_CASE("primitive_range_iteration_pop_back", "[primitive]")
{
    PrimitiveType TopType = PrimitiveType::Triangle;
    PrimitiveType BottomType = PrimitiveType::Edge;

    auto range2 = wmtk::utils::primitive_range(TopType, BottomType);
    {
        auto range = wmtk::utils::primitive_range_iter(TopType, BottomType);
        std::vector<PrimitiveType> range_vec(range.begin(), range.end());

        assert(range_vec == range2);
    }
    range2.pop_back();

    auto range = wmtk::utils::primitive_range_iter(TopType, BottomType + 1);
    std::vector<PrimitiveType> range_vec(range.begin(), range.end());

    REQUIRE(range_vec == range2);
}

TEST_CASE("primitive_range_iteration_from_one", "[primitive]")
{
    for (PrimitiveType pt = PrimitiveType::Edge; pt <= PrimitiveType::Tetrahedron; pt = pt + 1) {
        const auto primitive_range = wmtk::utils::primitive_below(pt);
        std::vector<wmtk::PrimitiveType> pts;
        for (size_t i = 1; i < primitive_range.size(); ++i) {
            pts.emplace_back(primitive_range[i]);
        }

        auto range = wmtk::utils::primitive_below_iter(pt - 1, false);
        std::vector<wmtk::PrimitiveType> range_vec(range.begin(), range.end());
        CHECK(pts == range_vec);
    }
    for (PrimitiveType pt = PrimitiveType::Vertex; pt <= PrimitiveType::Tetrahedron; pt = pt + 1) {
        PrimitiveType simplex_ptype = pt;
        for (PrimitiveType pt2 = PrimitiveType::Vertex; pt2 <= pt; pt2 = pt2 + 1) {
            PrimitiveType face_ptype = pt2;
            PrimitiveType start = simplex_ptype - 1;
            PrimitiveType end = face_ptype + 1;
            auto switch_tuple_types = wmtk::utils::primitive_range_iter(start, end);

            std::vector<PrimitiveType> switch_tuple_types_old =
                wmtk::utils::primitive_range(simplex_ptype, face_ptype);

            spdlog::info("{} => {} so {} {}", simplex_ptype, face_ptype, start, end);

            std::vector<PrimitiveType> mine;
            if (start >= end) {
                mine = {switch_tuple_types.begin(), switch_tuple_types.end()};
            }

            std::vector<PrimitiveType> target;
            for (size_t i = 1; i < switch_tuple_types_old.size() - 1; ++i) {
                target.emplace_back(switch_tuple_types_old[i]);
            }


            CHECK(mine == target);
        }
    }
}
