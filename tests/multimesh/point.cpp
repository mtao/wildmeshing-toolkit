#include "../tools/EdgeMesh_examples.hpp"
#include <wmtk/PointMesh.hpp>
#include "../tools/TriMesh_examples.hpp"

#include <catch2/catch_test_macros.hpp>

using namespace wmtk;



void test_point(Mesh& m) {

    auto vertices = m.get_all(PrimitiveType::Vertex);

    auto point_mesh = std::make_shared<wmtk::PointMesh>(vertices.size());

    std::vector<std::array<Tuple,2>> tups;
    for(size_t j = 0; j < vertices.size(); ++j) {
        tups.emplace_back(std::array<Tuple,2>{{Tuple(-1,-1,-1,j), vertices[j]}});
    }

    m.register_child_mesh(std::dynamic_pointer_cast<Mesh>(point_mesh), tups);


    for(const auto& p: vertices) {
        CHECK(m.can_map(*point_mesh,simplex::Simplex(PrimitiveType::Vertex,p)));
    }
    for(const auto& p: point_mesh->get_all(PrimitiveType::Vertex)) {
        CHECK(point_mesh->can_map(m,simplex::Simplex(PrimitiveType::Vertex,p)));
    }


}

TEST_CASE("test_point_trimesh", "[multimesh][2D]")
{
    auto trimesh = std::make_shared<TriMesh>(wmtk::tests::two_neighbors());
    test_point(*trimesh);
}
TEST_CASE("test_point_edgemesh", "[multimesh][2D]")
{
    auto edge_mesh = std::make_shared<EdgeMesh>(tests::single_line());
    test_point(*edge_mesh);
}
