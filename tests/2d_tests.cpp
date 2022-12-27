#include <wmtk/TriMesh.h>
#include <wmtk/utils/OperationLogger.h>
#include <nlohmann/json.hpp>
#include <wmtk/ExecutionScheduler.hpp>

#include <igl/read_triangle_mesh.h>
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
        REQUIRE(vertices_tuples[0].vid(m) == 0);
        REQUIRE(vertices_tuples[1].vid(m) == 1);
        REQUIRE(vertices_tuples[2].vid(m) == 2);
    }
    SECTION("test generation from faces")
    {
        auto faces_tuples = m.get_faces();
        REQUIRE(faces_tuples.size() == 1);

        // to test vid initialized correctly
        REQUIRE(faces_tuples[0].vid(m) == tris[0][0]);

        // // to test the fid is a triangle touching this vertex
        // std::vector<size_t> tris = m_vertex_connectivity[faces_tuples[0].vid(*this)].m_conn_tris;
        // REQUIRE(std::find(tris.begin(), tris.end(), faces_tuples[0].fid(*this)) != tris.end());
    }

    SECTION("test generation from edges")
    {
        auto edges_tuples = m.get_edges();
        REQUIRE(edges_tuples.size() == 3);
    }
}

TEST_CASE("test generate tuples with 2 triangle", "[test_tuple_generation]")
{
    // 	   v3     /
    //     / \    /
    // 	  /f1 \   /
    // v2 -----v1 /
    // 	  \f0 /   /
    //     \ /    /
    // 	    v0    /
    TriMesh m;
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{2, 1, 3}}};
    m.create_mesh(4, tris);

    SECTION("test generation from vertics")
    {
        auto vertices_tuples = m.get_vertices();
        REQUIRE(vertices_tuples.size() == 4);
        REQUIRE(vertices_tuples[0].vid(m) == 0);
        REQUIRE(vertices_tuples[1].vid(m) == 1);
        REQUIRE(vertices_tuples[2].vid(m) == 2);
        REQUIRE(vertices_tuples[3].vid(m) == 3);

        // test the faces are assigned correctly
        REQUIRE(vertices_tuples[1].fid(m) == 0);
        REQUIRE(vertices_tuples[2].fid(m) == 0);
    }

    SECTION("test generation from faces")
    {
        auto faces_tuples = m.get_faces();
        REQUIRE(faces_tuples.size() == 2);

        // std::vector<size_t> conn_tris =
        //     m_vertex_connectivity[faces_tuples[0].vid(*this)].m_conn_tris;
        // REQUIRE(
        //     std::find(conn_tris.begin(), conn_tris.end(), faces_tuples[0].fid(*this)) !=
        //     conn_tris.end());
    }

    SECTION("test generation from edges")
    {
        auto edges_tuples = m.get_edges();
        REQUIRE(edges_tuples.size() == 5);
        REQUIRE(edges_tuples[0].fid(m) == 0);
        REQUIRE(edges_tuples[1].fid(m) == 0);
        REQUIRE(edges_tuples[2].fid(m) == 0);
        REQUIRE(edges_tuples[3].fid(m) == 1);
        REQUIRE(edges_tuples[4].fid(m) == 1);
    }
}

// for every quiry do a require
TEST_CASE("random 10 switches on 2 traingles", "[tuple_operation]")
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
                case 0: v_tuple = v_tuple.switch_vertex(m); break;
                case 1: v_tuple = v_tuple.switch_edge(m); break;
                case 2: v_tuple = v_tuple.switch_face(m).value_or(v_tuple); break;
                default:;
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
                case 0: e_tuple = e_tuple.switch_vertex(m); break;
                case 1: e_tuple = e_tuple.switch_edge(m); break;
                case 2: e_tuple = e_tuple.switch_face(m).value_or(e_tuple); break;
                default:;
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
                case 0: f_tuple = f_tuple.switch_vertex(m); break;
                case 1: f_tuple = f_tuple.switch_edge(m); break;
                case 2: f_tuple = f_tuple.switch_face(m).value_or(f_tuple); break;
                default:;
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

bool tuple_equal(const TriMesh& m, TriMesh::Tuple t1, TriMesh::Tuple t2)
{
    return (t1.fid(m) == t2.fid(m)) && (t1.eid(m) == t2.eid(m)) && (t1.vid(m) == t2.vid(m));
}


// checking for every tuple t:
// (1) t.switch_vertex().switch_vertex() == t
// (2) t.switch_edge().switch_edge() == t
// (3) t.switch_tri().switch_tri() == t
TEST_CASE("double switches is identity", "[tuple_operation]")
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
            REQUIRE(tuple_equal(m, v_tuple_after, v_tuple));
            v_tuple_after = double_switch_edge(v_tuple, m);
            REQUIRE(tuple_equal(m, v_tuple_after, v_tuple));
            v_tuple_after = double_switch_face(v_tuple, m);
            REQUIRE(tuple_equal(m, v_tuple_after, v_tuple));
        }
    }

    SECTION("test all tuples generated using edges")
    {
        TriMesh::Tuple e_tuple_after;
        auto edges_tuples = m.get_edges();
        for (int i = 0; i < edges_tuples.size(); i++) {
            TriMesh::Tuple e_tuple = edges_tuples[i];
            e_tuple_after = double_switch_vertex(e_tuple, m);
            REQUIRE(tuple_equal(m, e_tuple_after, e_tuple));
            e_tuple_after = double_switch_edge(e_tuple, m);
            REQUIRE(tuple_equal(m, e_tuple_after, e_tuple));
            e_tuple_after = double_switch_face(e_tuple, m);
            REQUIRE(tuple_equal(m, e_tuple_after, e_tuple));
        }
    }

    SECTION("test all tuples generated using faces")
    {
        TriMesh::Tuple f_tuple_after;
        auto faces_tuples = m.get_faces();
        for (int i = 0; i < faces_tuples.size(); i++) {
            TriMesh::Tuple f_tuple = faces_tuples[i];
            f_tuple_after = double_switch_vertex(f_tuple, m);
            REQUIRE(tuple_equal(m, f_tuple_after, f_tuple));
            f_tuple_after = double_switch_edge(f_tuple, m);
            REQUIRE(tuple_equal(m, f_tuple_after, f_tuple));
            f_tuple_after = double_switch_face(f_tuple, m);
            REQUIRE(tuple_equal(m, f_tuple_after, f_tuple));
        }
    }
}
// check for every t
// t.switch_vertex().switchedge().switchvertex().switchedge().switchvertex().switchedge() == t
TEST_CASE("vertex_edge switches equals indentity", "[tuple_operation]")
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
            REQUIRE(tuple_equal(m, v_tuple, v_tuple_after));
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
            REQUIRE(tuple_equal(m, e_tuple, e_tuple_after));
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
            REQUIRE(tuple_equal(m, f_tuple, f_tuple_after));
        }
    }
}

TEST_CASE("test_link_check", "[test_pre_check]")
{
    TriMesh m;
    SECTION("extra_face_after_collapse")
    {
        std::vector<std::array<size_t, 3>> tris =
            {{{1, 2, 3}}, {{0, 1, 4}}, {{0, 2, 5}}, {{0, 1, 6}}, {{0, 2, 6}}, {{1, 2, 6}}};
        m.create_mesh(7, tris);
        TriMesh::Tuple edge(1, 2, 0, m);
        REQUIRE_FALSE(m.check_link_condition(edge));
    }
    SECTION("one_triangle")
    {
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}};
        m.create_mesh(3, tris);

        TriMesh::Tuple edge(0, 2, 0, m);
        assert(edge.is_valid(m));
        REQUIRE_FALSE(m.check_link_condition(edge));
    }
    SECTION("one_tet")
    {
        std::vector<std::array<size_t, 3>> tris = {
            {{0, 1, 2}},
            {{1, 3, 2}},
            {{0, 2, 3}},
            {{3, 0, 1}}};
        m.create_mesh(4, tris);

        TriMesh::Tuple edge(1, 0, 0, m);
        assert(edge.is_valid(m));
        REQUIRE_FALSE(m.check_link_condition(edge));
    }
    SECTION("non_manifold_after_collapse")
    {
        std::vector<std::array<size_t, 3>> tris = {
            {{0, 1, 5}},
            {{1, 2, 5}},
            {{2, 3, 5}},
            {{5, 3, 4}}};
        m.create_mesh(6, tris);

        TriMesh::Tuple fail_edge(5, 0, 1, m);
        REQUIRE_FALSE(m.check_link_condition(fail_edge));
        TriMesh::Tuple pass_edge(0, 2, 0, m);
        REQUIRE(m.check_link_condition(pass_edge));
    }
}
// test manifold (eid uniqueness)
TEST_CASE("test unique edge", "[test_2d_operation]")
{
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 3, 2}}, {{4, 1, 0}}, {{0, 2, 5}}};
    auto m = TriMesh();
    m.create_mesh(6, tris);
    REQUIRE(m.check_edge_manifold());
}

TEST_CASE("edge_collapse", "[test_2d_operation]")
{
    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 3, 2}}, {{4, 1, 0}}, {{0, 2, 5}}};
    SECTION("rollback")
    {
        class NoCollapseMesh : public TriMesh
        {
            bool collapse_edge_before(const TriMesh::Tuple& loc) override { return true; };
            bool collapse_edge_after(const TriMesh::Tuple& loc) override { return false; };
        };
        auto m = NoCollapseMesh();
        m.create_mesh(6, tris);
        const auto tuple = NoCollapseMesh::Tuple(1, 0, 0, m);
        REQUIRE(tuple.is_valid(m));
        std::vector<TriMesh::Tuple> dummy;
        REQUIRE_FALSE(m.collapse_edge(tuple, dummy));
        REQUIRE(tuple.is_valid(m));
    }
    SECTION("collapse")
    {
        class Collapse : public TriMesh
        {
            bool collapse_edge_before(const TriMesh::Tuple& loc) override { return true; };
            bool collapse_edge_after(const TriMesh::Tuple& loc) override { return true; };
        };
        auto m = Collapse();

        m.create_mesh(6, tris);
        const auto tuple = Collapse::Tuple(1, 0, 0, m);

        REQUIRE(tuple.is_valid(m));
        std::vector<TriMesh::Tuple> dummy;

        REQUIRE(m.collapse_edge(tuple, dummy)); // fail at check manifold
        REQUIRE_FALSE(tuple.is_valid(m));
    }
}

TEST_CASE("swap_operation", "[test_2d_operation]")
{
    SECTION("swap")
    {
        TriMesh m;
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
        m.create_mesh(4, tris);
        TriMesh::Tuple edge(0, 2, 0, m);
        assert(edge.is_valid(m));

        std::vector<TriMesh::Tuple> dummy;
        REQUIRE(m.swap_edge(edge, dummy));
    }
    SECTION("swap_boundary")
    {
        TriMesh m;
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
        m.create_mesh(4, tris);
        TriMesh::Tuple edge(0, 1, 0, m);
        assert(edge.is_valid(m));

        std::vector<TriMesh::Tuple> dummy;
        REQUIRE_FALSE(m.swap_edge(edge, dummy));
    }

    SECTION("swap_on_connected_vertices")
    {
        TriMesh m2;
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 0, 3}}, {{1, 3, 2}}};
        m2.create_mesh(4, tris);
        TriMesh::Tuple edge(0, 2, 0, m2);
        assert(edge.is_valid(m2));
        std::vector<TriMesh::Tuple> dummy;
        REQUIRE_FALSE(m2.swap_edge(edge, dummy));
    }

    SECTION("swap 4 times retain start tuple")
    {
        TriMesh m3;
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
        m3.create_mesh(4, tris);
        TriMesh::Tuple edge(0, 2, 0, m3);
        assert(edge.is_valid(m3));

        std::vector<TriMesh::Tuple> dummy;
        REQUIRE(m3.swap_edge(edge, dummy));
        auto new_t = dummy[0];
        assert(new_t.is_valid(m3));
        REQUIRE(m3.swap_edge(new_t, dummy));
        new_t = dummy[0];
        assert(new_t.is_valid(m3));
        REQUIRE(m3.swap_edge(new_t, dummy));
        new_t = dummy[0];
        assert(new_t.is_valid(m3));
        REQUIRE(m3.swap_edge(new_t, dummy));
        new_t = dummy[0];
        REQUIRE(new_t.vid(m3) == 0);
        REQUIRE(new_t.switch_vertex(m3).vid(m3) == 1);
        REQUIRE(new_t.switch_edge(m3).switch_vertex(m3).vid(m3) == 2);
    }
}

TEST_CASE("split_operation", "[test_2d_operation]")
{
    TriMesh m;
    SECTION("1_tri_split")
    {
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}};
        m.create_mesh(3, tris);
        auto edges = m.get_edges();
        TriMesh::Tuple edge(0, 1, 0, m);
        std::vector<TriMesh::Tuple> dummy;
        assert(edge.is_valid(m));
        REQUIRE(m.split_edge(edge, dummy));
        REQUIRE_FALSE(edges[0].is_valid(m));
    }
    SECTION("2_tris_split")
    {
        std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{1, 3, 2}}};
        m.create_mesh(4, tris);
        auto edges = m.get_edges();
        TriMesh::Tuple edge(1, 0, 0, m);
        std::vector<TriMesh::Tuple> dummy;
        assert(edge.is_valid(m));
        REQUIRE(m.split_edge(edge, dummy));
        for (auto e : edges) REQUIRE_FALSE(e.is_valid(m));
    }
}


TEST_CASE("replay_operations", "[test_2d_operation]")
{
    TriMesh m;


    // note this doesn't fix deleted element state
    auto copy_tri_mesh = [](const TriMesh& source, TriMesh& destination) {
        auto face_tuples = source.get_faces();
        std::vector<std::array<size_t, 3>> faces;
        faces.reserve(face_tuples.size());
        std::transform(
            face_tuples.begin(),
            face_tuples.end(),
            std::back_inserter(faces),
            [&](const TriMesh::Tuple& tup) -> std::array<size_t, 3> {
                return source.oriented_tri_vids(tup);
            });

        destination.create_mesh(source.vert_capacity(), faces);
    };

    auto edge_vids = [&](const TriMesh::Tuple& edge) -> std::array<size_t, 2> {
        std::array<size_t, 2> ret;
        ret[0] = edge.vid(m);
        TriMesh::Tuple other = edge.switch_vertex(m);
        ret[1] = other.vid(m);
        return ret;
    };

    auto check_vertex_indices = [&](const TriMesh::Tuple& tuple, const std::set<size_t>& expected) {
        REQUIRE(tuple.is_valid(m));
        int size = expected.size();
        std::set<size_t> orig;
        if (size == 3) {
            auto tri_vids = m.oriented_tri_vids(tuple);
            orig = {tri_vids.begin(), tri_vids.end()};
        } else if (size == 2) {
            auto e_vids = edge_vids(tuple);
            orig = {e_vids.begin(), e_vids.end()};
        }
        REQUIRE(orig == expected);
    };

    auto check_face_equality = [&](const TriMesh& a, const TriMesh& b) {
        auto a_face_tuples = a.get_faces();
        auto b_face_tuples = b.get_faces();
        REQUIRE(a_face_tuples.size() == b_face_tuples.size());
        for (size_t j = 0; j < a_face_tuples.size(); ++j) {
            const auto& a_tup = a_face_tuples[j];
            const auto& b_tup = b_face_tuples[j];
            REQUIRE(a_tup.as_stl_tuple() == b_tup.as_stl_tuple());
        }
    };

    std::vector<std::array<size_t, 3>> tris = {{{0, 1, 2}}, {{3, 1, 0}}};
    m.create_mesh(4, tris);

    TriMesh initial_mesh;
    copy_tri_mesh(m, initial_mesh);


    {
        // 0-----2
        // |\    |
        // | \   |
        // |  \  |
        // |   \ |
        // |    \|
        // 3-----1
        auto face_tuples = m.get_faces();
        REQUIRE(face_tuples.size() == 2);
        check_vertex_indices(face_tuples[0], {0, 1, 2});
        check_vertex_indices(face_tuples[1], {0, 1, 3});
    }


    // We will simultaneously track operations and run them to validate the logger
    std::vector<std::pair<std::string, TriMesh::Tuple>> operations;

    std::stringstream output;
    OperationLogger op_logger(output);


    m.p_operation_logger = &op_logger;
    // record do a flip, split, and then collapse
    SECTION("logging_operations")
    {
        TriMesh::Tuple edge(0, 2, 0, m);

        std::vector<TriMesh::Tuple> dummy;


        REQUIRE(edge.is_valid(m));
        REQUIRE(m.swap_edge(edge, dummy));
        operations.emplace_back("edge_swap", edge);
        {
            // 0-----2
            // |    /|
            // |   / |
            // |  /  |
            // | /   |
            // |/    |
            // 3-----1
            auto face_tuples = m.get_faces();
            REQUIRE(face_tuples.size() == 2);
            check_vertex_indices(face_tuples[0], {0, 2, 3});
            check_vertex_indices(face_tuples[1], {1, 2, 3});
        }


        // flip the 2,3 edge
        edge = TriMesh::Tuple(2, 0, 0, m);
        edge.print_info();
        REQUIRE(edge.is_valid(m));
        check_vertex_indices(edge, {2, 3});

        REQUIRE(m.split_edge(edge, dummy));
        operations.emplace_back("edge_split", edge);
        {
            // 0-----2
            // |\   /|
            // | \ / |
            // |  4  |
            // | / \ |
            // |/   \|
            // 3-----1
            auto face_tuples = m.get_faces();
            REQUIRE(face_tuples.size() == 4);
            check_vertex_indices(face_tuples[0], {0, 4, 2});
            check_vertex_indices(face_tuples[1], {4, 1, 2});
            check_vertex_indices(face_tuples[2], {0, 3, 4});
            check_vertex_indices(face_tuples[3], {3, 1, 4});
        }


        // collapse 4 into 2
        edge = TriMesh::Tuple(2, 0, 0, m);
        edge.print_info();
        REQUIRE(edge.is_valid(m));
        check_vertex_indices(edge, {2, 4});

        REQUIRE(m.collapse_edge(edge, dummy));
        operations.emplace_back("collapse_edge", edge);
        {
            // 0      .
            // |\     .
            // | \    .
            // |  5   .
            // | / \  .
            // |/   \ .
            // 3-----1
            // (dots are to remove multi-line comment warnings)
            auto face_tuples = m.get_faces();
            REQUIRE(face_tuples.size() == 2);
            check_vertex_indices(face_tuples[0], {0, 3, 5});
            check_vertex_indices(face_tuples[1], {1, 3, 5});
        }
    }

    TriMesh final_mesh;
    copy_tri_mesh(m, final_mesh);


    copy_tri_mesh(initial_mesh, m);

    std::vector<std::pair<std::string, TriMesh::Tuple>> recorded_operations;
    SECTION("logged operations")
    {
        for (std::string line; std::getline(output, line);) {
            nlohmann::json js = nlohmann::json::parse(line);

            auto& [op_name, tup] = recorded_operations.emplace_back();
            const auto& arr = js["tuple"];
            REQUIRE(arr.size() == 3);
            tup = TriMesh::Tuple(arr[0], arr[1], arr[2], m);
            op_name = js["operation"];
        }


        REQUIRE(operations.size() == recorded_operations.size());
        for (size_t j = 0; j < operations.size(); ++j) {
            const auto& [op, tup] = operations[j];
            const auto& [rec_op, rec_tup] = recorded_operations[j];
            CHECK(op == rec_op);
            CHECK(tup.as_stl_tuple() == rec_tup.as_stl_tuple());
        }

        ExecutePass<TriMesh, ExecutionPolicy::kSeq> scheduler;
        scheduler(m, recorded_operations);

        check_face_equality(m, final_mesh);
    }


    SECTION("replay")
    {
        // TODO:
    }
}
