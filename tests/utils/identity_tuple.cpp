#include <catch2/catch_test_macros.hpp>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>

#include "tools/TetMesh_examples.hpp"
#include "tools/TriMesh_examples.hpp"

#include "tools/EdgeMesh_examples.hpp"
#include <wmtk/utils/internal/identity_tuple.hpp>

using namespace wmtk;
using namespace wmtk::tests;
using namespace operations;

constexpr PrimitiveType PV = PrimitiveType::Vertex;
constexpr PrimitiveType PE = PrimitiveType::Edge;
constexpr PrimitiveType PF = PrimitiveType::Triangle;
constexpr PrimitiveType PT = PrimitiveType::Tetrahedron;


namespace {
void check(const Mesh& m) {
    auto pt = m.top_simplex_type();
    auto g = m.get_all(pt);

    for(size_t j = 0; j < g.size(); ++j) {
        CHECK(wmtk::utils::internal::identity_tuple(pt,j) == g[j]);
    }
}
}

TEST_CASE("test_debug_free", "[mesh]")
{
    check(single_line());
    check(two_segments());
    check(multiple_lines());
    check(two_line_loop());

    check(single_triangle());
    check(*disk(3));

    // TODO: tetmesh doens't return the identity tuple
    //check(tests_3d::single_tet());
    //check(tests_3d::two_by_two_by_two_grids_tets());
}
