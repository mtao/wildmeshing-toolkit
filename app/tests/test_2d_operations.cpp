#include <stdlib.h>
#include <wmtk/TriMesh.h>
#include <catch2/catch.hpp>
#include <iostream>
#include "EdgeCollapse/EdgeCollapse.h"

using namespace wmtk;

// these roll back tests must have more than 2 triangles, since by design when there are only two
// incident triangles the edge cannot be collapsed
// this check should be doen in pre collapse check

TEST_CASE("edge_collapse", "[test_2d_operation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}};
    m.create_mesh(6, tris);

    TriMesh::Tuple tuple = TriMesh::Tuple(1, 0, 0, m);
    TriMesh::Tuple dummy;
    assert(false);
    bool collapse = m.collapse_edge(tuple, dummy);

    std::cout << collapse << std::endl;
    REQUIRE(collapse);
    REQUIRE_FALSE(tuple.is_valid(m));
    REQUIRE(m.check_mesh_connectivity_validity());
}

TEST_CASE("rollback_collapse_operation", "[test_2d_operation]")
{
    class NoSplitMesh : public TriMesh
    {
        bool collapse_after(const TriMesh::Tuple& loc) override { return false; };
    };
    auto m = NoSplitMesh();
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}};
    m.create_mesh(6, tris);

    const auto tuple = NoSplitMesh::Tuple(1, 0, 0, m);
    TriMesh::Tuple dummy;
    REQUIRE_FALSE(m.collapse_edge(tuple, dummy));
    REQUIRE(tuple.is_valid(m));
}

TEST_CASE("shortest_edge_collapse", "[test_2d_operations]")
{
    using namespace Edge2d;
    // 0___1___2    0 __1___2
    // \  /\  /      \  |  /
    // 3\/__\/4  ==>  \ | /
    //   \  /          \|/6
    //    \/5


    std::vector<Eigen::Vector3d> v_positions(6);
    v_positions[0] = Eigen::Vector3d(-3, 3, 0);
    v_positions[1] = Eigen::Vector3d(0, 3, 0);
    v_positions[2] = Eigen::Vector3d(3, 3, 0);
    v_positions[3] = Eigen::Vector3d(0, 0, 0);
    v_positions[4] = Eigen::Vector3d(0.5, 0, 0);
    v_positions[5] = Eigen::Vector3d(0, -3, 0);
    EdgeCollapse m(v_positions);
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}};
    m.create_mesh(6, tris);
    m.collapse_shortest();
    REQUIRE(m.n_vertices() == 6);
    REQUIRE(m.n_triangles() == 4);
    // the collapsed edge tuple is not valid anymore
    std::vector<TriMesh::Tuple> edges = m.get_edges();
    // find the shortest edge
    double shortest = 100;
    TriMesh::Tuple shortest_edge;
    for (TriMesh::Tuple t : edges) {
        size_t v1 = t.get_vid();
        size_t v2 = m.switch_vertex(t).get_vid();
        if ((v_positions[v1] - v_positions[v2]).squaredNorm() < shortest) {
            shortest = (v_positions[v1] - v_positions[v2]).squaredNorm();
            shortest_edge = t;
        }
    }
    m.collapse_shortest();
    REQUIRE_FALSE(shortest_edge.is_valid(m));
}
