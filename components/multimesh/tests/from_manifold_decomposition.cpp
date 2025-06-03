
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>
#include <wmtk/components/multimesh/from_manifold_decomposition.hpp>
#include <wmtk/Mesh.hpp>


TEST_CASE("from_manifold_decomposition", "[components][multimesh][dart]")
{
    {
        wmtk::RowVectors2l S(4, 2);
        S.row(0) << 0, 1;
        S.row(1) << 1, 2;
        S.row(2) << 2, 3;
        S.row(3) << 2, 4;
        //auto mptr = wmtk::Mesh::from_vertex_indices(S);
        auto meshes = wmtk::components::multimesh::from_manifold_decomposition(S, true);

        REQUIRE(meshes.size() == 2);
        auto& em = *meshes[0];
        auto& pm = *meshes[1];

        CHECK(em.get_all(wmtk::PrimitiveType::Edge).size() == 4);
        CHECK(em.get_all(wmtk::PrimitiveType::Vertex).size() == 7);
        CHECK(pm.get_all(wmtk::PrimitiveType::Vertex).size() == 3);



    }

    {
        wmtk::RowVectors3l S(5, 3);
        S.row(0) << 0, 1, 2;//3 edges // 3vert
        S.row(1) << 1, 2, 3;//+2 edges (5) //+1 vert (4)
        S.row(2) << 4, 2, 3;//+2 edges (7) //+1 vert (5)
        S.row(3) << 4, 2, 5;//+3 edges(10) // +3 
        S.row(4) << 4, 2, 6;//+3 edges(13) // +3
        auto meshes = wmtk::components::multimesh::from_manifold_decomposition(S, true);

        REQUIRE(meshes.size() == 2);
        auto& tm = *meshes[0];
        auto& em = *meshes[1];

        CHECK(tm.get_all(wmtk::PrimitiveType::Triangle).size() == 5);
        CHECK(tm.get_all(wmtk::PrimitiveType::Edge).size() == 13);
        CHECK(tm.get_all(wmtk::PrimitiveType::Vertex).size() == 11);

        CHECK(em.get_all(wmtk::PrimitiveType::Edge).size() == 3);
        CHECK(em.get_all(wmtk::PrimitiveType::Vertex).size() == 6);
    }


    {
        wmtk::RowVectors3l S(9, 3);
        S.row(0) << 0, 1, 4;
        S.row(1) << 0, 1, 5;
        S.row(2) << 0, 1, 6;
        S.row(3) << 0, 2, 7;
        S.row(4) << 0, 2, 8;
        S.row(5) << 0, 2, 9;
        S.row(6) << 0, 3, 10;
        S.row(7) << 0, 3, 11;
        S.row(8) << 0, 3, 12;
        auto meshes = wmtk::components::multimesh::from_manifold_decomposition(S, true);

        REQUIRE(meshes.size() == 3);
        auto& tm = *meshes[0];
        auto& em = *meshes[1];
        auto& vm = *meshes[2];

        // mesh becomes completely disconnected
        CHECK(tm.get_all(wmtk::PrimitiveType::Triangle).size() == 9);
        CHECK(tm.get_all(wmtk::PrimitiveType::Edge).size() == 27);
        CHECK(tm.get_all(wmtk::PrimitiveType::Vertex).size() == 27);

        // should output 3 copies the 3 edges that are nonmanifold
        CHECK(em.get_all(wmtk::PrimitiveType::Edge).size() == 9);
        CHECK(em.get_all(wmtk::PrimitiveType::Vertex).size() == 18);

        // the 3 edges met at a single vertex, resulting in 3 copies
        CHECK(vm.get_all(wmtk::PrimitiveType::Vertex).size() == 3);
    }
}
