
#include <catch2/catch_test_macros.hpp>
#include <wmtk/EdgeMeshOperationExecutor.hpp>
#include <wmtk/multimesh/from_facet_orientations.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/operations/utils/multi_mesh_edge_split.hpp>
#include "../tools/DEBUG_EdgeMesh.hpp"
#include "../tools/is_free.hpp"
#include "tools/EdgeMesh_examples.hpp"

using namespace wmtk;
using namespace wmtk::simplex;
using namespace wmtk::tests;
using namespace operations;


TEST_CASE("split_no_topology_edgemesh", "[operations][split]")
{
    const int64_t initial_size = 20;
    DEBUG_EdgeMesh m = [](int64_t size) {
        EdgeMesh m;
        m.initialize_free(size);
        return m;
    }(initial_size);
    int64_t size = initial_size;
    for (Tuple edge : m.get_all(PrimitiveType::Edge)) {
        EdgeSplit op(m);
        REQUIRE(!op(simplex::Simplex(m, PrimitiveType::Edge, edge)).empty());
        size++;
        REQUIRE(m.is_connectivity_valid());
        REQUIRE(is_free(m));
        CHECK(m.get_all(PrimitiveType::Edge).size() == size);
    }
}
TEST_CASE("split_edge_with_child_points", "[operations][split]")
{
    const int64_t initial_size = 20;


    auto em = std::make_shared<EdgeMesh>(single_line());

    auto pm = std::make_shared<PointMesh>(2);

    em->register_child_mesh(
        pm,
        wmtk::multimesh::from_facet_orientations(*em, *pm, em->get_all(PrimitiveType::Vertex)));


    REQUIRE(multimesh::utils::check_maps_valid(*em));
    REQUIRE(multimesh::utils::check_maps_valid(*pm));

    {
        auto scope = em->create_scope();
        auto only_edge_tup = em->get_all(PrimitiveType::Edge)[0];
        auto s = simplex::NavigatableSimplex(*em, PrimitiveType::Edge, only_edge_tup);
        operations::utils::multi_mesh_edge_split(*em, s, {});
    }

    REQUIRE(multimesh::utils::check_maps_valid(*em));
    REQUIRE(multimesh::utils::check_maps_valid(*pm));
}
