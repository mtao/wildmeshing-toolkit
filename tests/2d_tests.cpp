#include <wmtk/TriMesh.h>

#include <stdlib.h>
#include <catch2/catch.hpp>
#include <iostream>

using namespace wmtk;

TEST_CASE("load mesh and create TriMesh", "[test_mesh_creation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}};
    m.create_mesh(3, tris);
    REQUIRE(m.tri_capacity() == tris.size());
    REQUIRE(m.vert_capacity() == 3);
}

TEST_CASE("test generate tuples with 1 triangle", "[test_tuple_generation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}};
    m.create_mesh(3, tris);

    SECTION("test generation from vertics")
    {
        auto vertices_tuples = m.get_vertices();
        REQUIRE(vertices_tuples.size() == 3);
        REQUIRE(vertices_tuples[0].vid() == 0);
        REQUIRE(vertices_tuples[1].vid() == 1);
        REQUIRE(vertices_tuples[2].vid() == 2);
    }
    SECTION("test generation from faces")
    {
        auto faces_tuples = m.get_faces();
        REQUIRE(faces_tuples.size() == 1);

        // to test vid initialized correctly
        REQUIRE(faces_tuples[0].vid() == tris[0][0]);

        // // to test the fid is a triangle touching this vertex
        // std::vector<size_t> tris = m_vertex_connectivity[faces_tuples[0].vid()].m_conn_tris;
        // REQUIRE(std::find(tris.begin(), tris.end(), faces_tuples[0].fid()) != tris.end());
    }

    SECTION("test generation from edges")
    {
        auto edges_tuples = m.get_edges();
        REQUIRE(edges_tuples.size() == 3);
    }
}

TEST_CASE("test generate tuples with 2 triangle", "[test_tuple_generation]")
{
    // 	   v3
    //     / \
    // 	  /f1 \
    // v2 -----v1
    // 	  \f0 /
    //     \ /
    // 	    v0
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
    m.create_mesh(4, tris);

    SECTION("test generation from vertics")
    {
        auto vertices_tuples = m.get_vertices();
        REQUIRE(vertices_tuples.size() == 4);
        REQUIRE(vertices_tuples[0].vid() == 0);
        REQUIRE(vertices_tuples[1].vid() == 1);
        REQUIRE(vertices_tuples[2].vid() == 2);
        REQUIRE(vertices_tuples[3].vid() == 3);

        // test the faces are assigned correctly
        REQUIRE(vertices_tuples[1].fid() == 0);
        REQUIRE(vertices_tuples[2].fid() == 0);
    }

    SECTION("test generation from faces")
    {
        auto faces_tuples = m.get_faces();
        REQUIRE(faces_tuples.size() == 2);

        // std::vector<size_t> conn_tris =
        //     m_vertex_connectivity[faces_tuples[0].vid()].m_conn_tris;
        // REQUIRE(
        //     std::find(conn_tris.begin(), conn_tris.end(), faces_tuples[0].fid()) !=
        //     conn_tris.end());
    }

    SECTION("test generation from edges")
    {
        auto edges_tuples = m.get_edges();
        REQUIRE(edges_tuples.size() == 5);
        REQUIRE(edges_tuples[0].fid() == 0);
        REQUIRE(edges_tuples[1].fid() == 0);
        REQUIRE(edges_tuples[2].fid() == 0);
        REQUIRE(edges_tuples[3].fid() == 1);
        REQUIRE(edges_tuples[4].fid() == 1);
    }
}

// for every quiry do a require
TEST_CASE("random 10 switches on 2 traingles", "[test_operation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
    m.create_mesh(4, tris);

    SECTION("test all tuples generated using vertices")
    {
        auto vertices_tuples = m.get_vertices();
        for (int i = 0; i < vertices_tuples.size(); i++) {
            TriMesh::Tuple v_tuple = vertices_tuples[i];
            for (int j = 0; j < 10; j++) {
                size_t test = rand() % 3;
                switch (test) {
                case 0: v_tuple = v_tuple.switch_vertex(m);
                case 1: v_tuple = v_tuple.switch_edge(m);
                case 2: v_tuple = v_tuple.switch_face(m).value_or(v_tuple);
                }
            }
            REQUIRE(v_tuple.is_valid(m));
        }
    }

    SECTION("test all tuples generated using edges")
    {
        auto edges_tuples = m.get_edges();
        for (int i = 0; i < edges_tuples.size(); i++) {
            TriMesh::Tuple e_tuple = edges_tuples[i];
            for (int j = 0; j < 10; j++) {
                size_t test = rand() % 3;
                switch (test) {
                case 0: e_tuple = e_tuple.switch_vertex(m);
                case 1: e_tuple = e_tuple.switch_edge(m);
                case 2: e_tuple = e_tuple.switch_face(m).value_or(e_tuple);
                }
            }
            REQUIRE(e_tuple.is_valid(m));
        }
    }

    SECTION("test all tuples generated using faces")
    {
        auto faces_tuples = m.get_faces();
        for (int i = 0; i < faces_tuples.size(); i++) {
            TriMesh::Tuple f_tuple = faces_tuples[i];
            for (int j = 0; j < 10; j++) {
                size_t test = rand() % 3;
                switch (test) {
                case 0: f_tuple = f_tuple.switch_vertex(m);
                case 1: f_tuple = f_tuple.switch_edge(m);
                case 2: f_tuple = f_tuple.switch_face(m).value_or(f_tuple);
                }
            }
            REQUIRE(f_tuple.is_valid(m));
        }
    }
}

TriMesh::Tuple double_switch_vertex(TriMesh::Tuple t, TriMesh& m)
{
    TriMesh::Tuple t_after = t.switch_vertex(m);
    t_after = t_after.switch_vertex(m);
    return t_after;
}

TriMesh::Tuple double_switch_edge(TriMesh::Tuple t, TriMesh& m)
{
    TriMesh::Tuple t_after = t.switch_edge(m);
    t_after = t_after.switch_edge(m);
    return t_after;
}

TriMesh::Tuple double_switch_face(TriMesh::Tuple t, TriMesh& m)
{
    TriMesh::Tuple t_after = t.switch_face(m).value_or(t);
    t_after = t_after.switch_face(m).value_or(t);
    return t_after;
}

bool tuple_equal(TriMesh::Tuple t1, TriMesh::Tuple t2)
{
    if (t1.vid() != t2.vid()) std::cout << "vids : " << t1.vid() << " " << t2.vid() << std::endl;
    if (t1.eid() != t2.eid()) std::cout << "eids : " << t1.eid() << " " << t2.eid() << std::endl;
    if (t1.fid() != t2.fid()) std::cout << "fids : " << t1.fid() << " " << t2.fid() << std::endl;

    return (t1.fid() == t2.fid()) && (t1.eid() == t2.eid()) && (t1.vid() == t2.vid());
}


// checking for every tuple t:
// (1) t.switch_vertex().switch_vertex() == t
// (2) t.switch_edge().switch_edge() == t
// (3) t.switch_tri().switch_tri() == t
TEST_CASE("double switches is identity", "[test_operation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
    m.create_mesh(4, tris);

    SECTION("test all tuples generated using vertices")
    {
        TriMesh::Tuple v_tuple_after;
        auto vertices_tuples = m.get_vertices();
        for (int i = 0; i < vertices_tuples.size(); i++) {
            TriMesh::Tuple v_tuple = vertices_tuples[i];
            v_tuple_after = double_switch_vertex(v_tuple, m);
            REQUIRE(tuple_equal(v_tuple_after, v_tuple));
            v_tuple_after = double_switch_edge(v_tuple, m);
            REQUIRE(tuple_equal(v_tuple_after, v_tuple));
            v_tuple_after = double_switch_face(v_tuple, m);
            REQUIRE(tuple_equal(v_tuple_after, v_tuple));
        }
    }

    SECTION("test all tuples generated using edges")
    {
        TriMesh::Tuple e_tuple_after;
        auto edges_tuples = m.get_edges();
        for (int i = 0; i < edges_tuples.size(); i++) {
            TriMesh::Tuple e_tuple = edges_tuples[i];
            e_tuple_after = double_switch_vertex(e_tuple, m);
            REQUIRE(tuple_equal(e_tuple_after, e_tuple));
            e_tuple_after = double_switch_edge(e_tuple, m);
            REQUIRE(tuple_equal(e_tuple_after, e_tuple));
            e_tuple_after = double_switch_face(e_tuple, m);
            REQUIRE(tuple_equal(e_tuple_after, e_tuple));
        }
    }

    SECTION("test all tuples generated using faces")
    {
        TriMesh::Tuple f_tuple_after;
        auto faces_tuples = m.get_faces();
        for (int i = 0; i < faces_tuples.size(); i++) {
            TriMesh::Tuple f_tuple = faces_tuples[i];
            f_tuple_after = double_switch_vertex(f_tuple, m);
            REQUIRE(tuple_equal(f_tuple_after, f_tuple));
            f_tuple_after = double_switch_edge(f_tuple, m);
            REQUIRE(tuple_equal(f_tuple_after, f_tuple));
            f_tuple_after = double_switch_face(f_tuple, m);
            REQUIRE(tuple_equal(f_tuple_after, f_tuple));
        }
    }
}
// check for every t
// t.switch_vertex().switchedge().switchvertex().switchedge().switchvertex().switchedge() == t
TEST_CASE("vertex_edge switches equals indentity", "[test_operation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
    m.create_mesh(4, tris);

    SECTION("test all tuples generated using vertices")
    {
        TriMesh::Tuple v_tuple_after;
        auto vertices_tuples = m.get_vertices();
        for (int i = 0; i < vertices_tuples.size(); i++) {
            TriMesh::Tuple v_tuple = vertices_tuples[i];
            v_tuple_after = v_tuple.switch_vertex(m);
            v_tuple_after = v_tuple_after.switch_edge(m);
            v_tuple_after = v_tuple_after.switch_vertex(m);
            v_tuple_after = v_tuple_after.switch_edge(m);
            v_tuple_after = v_tuple_after.switch_vertex(m);
            v_tuple_after = v_tuple_after.switch_edge(m);
            REQUIRE(tuple_equal(v_tuple, v_tuple_after));
        }
    }

    SECTION("test all tuples generated using edges")
    {
        TriMesh::Tuple e_tuple_after;
        auto edges_tuples = m.get_edges();
        for (int i = 0; i < edges_tuples.size(); i++) {
            TriMesh::Tuple e_tuple = edges_tuples[i];
            e_tuple_after = e_tuple.switch_vertex(m);
            e_tuple_after = e_tuple_after.switch_edge(m);
            e_tuple_after = e_tuple_after.switch_vertex(m);
            e_tuple_after = e_tuple_after.switch_edge(m);
            e_tuple_after = e_tuple_after.switch_vertex(m);
            e_tuple_after = e_tuple_after.switch_edge(m);
            REQUIRE(tuple_equal(e_tuple, e_tuple_after));
        }
    }

    SECTION("test all tuples generated using faces")
    {
        TriMesh::Tuple f_tuple_after;
        auto faces_tuples = m.get_faces();
        for (int i = 0; i < faces_tuples.size(); i++) {
            TriMesh::Tuple f_tuple = faces_tuples[i];
            f_tuple_after = f_tuple.switch_vertex(m);
            f_tuple_after = f_tuple_after.switch_edge(m);
            f_tuple_after = f_tuple_after.switch_vertex(m);
            f_tuple_after = f_tuple_after.switch_edge(m);
            f_tuple_after = f_tuple_after.switch_vertex(m);
            f_tuple_after = f_tuple_after.switch_edge(m);
            REQUIRE(tuple_equal(f_tuple, f_tuple_after));
        }
    }
}

TEST_CASE("link_check", "[test_pre_check]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris =
        {{{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}, {{0, 1, 6}}, {{0, 2, 6}}, {{1, 2, 6}}};
    m.create_mesh(7, tris);
    TriMesh::Tuple edge(1, 2, 0, m);
    REQUIRE_FALSE(m.check_link_condition(edge));
}


// these roll back tests must have more than 2 triangles, since by design when there are only two
// incident triangles the edge cannot be collapsed
// this check should be doen in pre collapse check

TEST_CASE("rollback_edge_collapse", "[test_2d_operation]")
{
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}};
    m.create_mesh(4, tris);

    TriMesh::Tuple tuple = TriMesh::Tuple(1, 0, 0, m);
    TriMesh::Tuple dummy;
    REQUIRE(tuple.is_valid(m));
    REQUIRE(m.check_link_condition(tuple));
    REQUIRE_FALSE(m.collapse_edge(tuple, dummy));
    REQUIRE(tuple.is_valid(m));
    REQUIRE(m.check_mesh_connectivity_validity());
}

TEST_CASE("collapse_operation", "[test_2d_operation]")
{
    class NoSplitMesh : public TriMesh
    {
        bool collapse_before(const TriMesh::Tuple& loc) override { return true; };
        bool collapse_after(const TriMesh::Tuple& loc) override { return true; };
    };
    auto m = NoSplitMesh();
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}};
    m.create_mesh(6, tris);
    const auto tuple = NoSplitMesh::Tuple(1, 0, 0, m);
    REQUIRE(tuple.is_valid(m));
    TriMesh::Tuple dummy;
    REQUIRE(m.collapse_edge(tuple, dummy));
    REQUIRE_FALSE(tuple.is_valid(m));
}