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
    auto below_it = wmtk::utils::template primitive_below_iter<l2u>(pt);

    std::vector<PrimitiveType> below_it_vec(below_it.begin(), below_it.end());
    CHECK(below == below_it_vec);
}
template <PrimitiveType pt, bool l2u>
void test_above()
{
    auto above = wmtk::utils::primitive_above(pt, l2u);
    auto above_it = wmtk::utils::template primitive_above_iter<l2u>(pt);

    std::vector<PrimitiveType> above_it_vec(above_it.begin(), above_it.end());
    CHECK(above == above_it_vec);
}
template <PrimitiveType Start, PrimitiveType End>
void test_range()
{
    auto range = wmtk::utils::primitive_range(Start, End);
    auto range_it = wmtk::utils::template primitive_range_iter<(Start <= End)>(Start, End);

    std::vector<PrimitiveType> range_it_vec(range_it.begin(), range_it.end());
    CHECK(range == range_it_vec);
}
} // namespace

TEST_CASE("primitive_range_iter", "[primitive]")
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
