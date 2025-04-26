#include <wmtk/components/multimesh/from_vertex_simplices.hpp>
#include <wmtk/components/multimesh/vertex_fusion.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>


#include <catch2/catch_test_macros.hpp>

#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>

TEST_CASE("from_vertex_simplices", "[components][multimesh][.]")
{
    wmtk::MatrixXl S(2, 3);
    S.row(0) << 0, 1, 2;
    S.row(1) << 2, 1, 3;

    auto f = std::make_shared<wmtk::TriMesh>();

    f->initialize(S);
    wmtk::MatrixXl E(2, 2);
    E.row(0) << 2, 1;
    E.row(1) << 3, 2;

    auto e = wmtk::components::multimesh::from_vertex_simplices(*f, E);
    CHECK(e->get_all(wmtk::PrimitiveType::Vertex).size() == 3);
    CHECK(e->get_all(wmtk::PrimitiveType::Edge).size() == 2);

    REQUIRE(wmtk::multimesh::utils::check_child_maps_valid(*f));
    auto s2 = wmtk::components::multimesh::from_vertex_simplices(*f, S);
    CHECK(s2->get_all(wmtk::PrimitiveType::Vertex).size() == 4);
    CHECK(s2->get_all(wmtk::PrimitiveType::Edge).size() == 5);
    CHECK(s2->get_all(wmtk::PrimitiveType::Triangle).size() == 2);

    // e->map_to_parent(wmtk
}
