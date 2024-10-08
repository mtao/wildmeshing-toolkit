#include <catch2/catch_test_macros.hpp>

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/multimesh/MultiMeshSimplexEventVisitor.hpp>
#include <wmtk/multimesh/MultiMeshSimplexVisitor.hpp>
#include <wmtk/multimesh/same_simplex_dimension_surjection.hpp>
#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include "../tools/DEBUG_TriMesh.hpp"
#include "../tools/TriMesh_examples.hpp"

#include <wmtk/utils/Logger.hpp>

using namespace wmtk;
using namespace wmtk::simplex;
using namespace wmtk::tests;


namespace {
struct PrintTypeSizeFunctor
{
    int operator()(const Mesh&, const Simplex&) const
    {
        logger().error("Unimplemented!");
        return 0;
    }
    int64_t operator()(const TriMesh& m, const Simplex&) const
    {
        logger().trace(
            "TriMesh: {} (path: {})",
            m.capacity(PrimitiveType::Triangle),
            m.absolute_multi_mesh_id());
        return 0.0;
    }
};

struct GetTypeSizeFunctorWithReturn
{
    std::string operator()(const Mesh&, const Simplex&) const { return "Unimplemented!"; }
    std::string operator()(const TriMesh& m, const Simplex&) const
    {
        return fmt::format(
            "[TriMesh: {} (path: {})]",
            m.capacity(PrimitiveType::Triangle),
            m.absolute_multi_mesh_id());
    }
};

struct PrintEdgeReturnsFunctor
{
    void operator()(
        const Mesh&,
        const Simplex&,
        const std::string& a,
        const Mesh&,
        const Simplex&,
        const std::string& b) const
    {
        logger().error("[{}] => [{}]", a, b);
    }
};
} // namespace


using TM = TriMesh;

constexpr PrimitiveType PV = PrimitiveType::Vertex;
constexpr PrimitiveType PE = PrimitiveType::Edge;
constexpr PrimitiveType PF = PrimitiveType::Triangle;


TEST_CASE("test_multi_mesh_print_visitor", "[multimesh][2D]")
{
    DEBUG_TriMesh parent = two_neighbors();
    std::shared_ptr<DEBUG_TriMesh> child0_ptr = std::make_shared<DEBUG_TriMesh>(single_triangle());
    std::shared_ptr<DEBUG_TriMesh> child1_ptr = std::make_shared<DEBUG_TriMesh>(one_ear());
    std::shared_ptr<DEBUG_TriMesh> child2_ptr =
        std::make_shared<DEBUG_TriMesh>(two_neighbors_cut_on_edge01());

    auto& child0 = *child0_ptr;
    auto& child1 = *child1_ptr;
    auto& child2 = *child2_ptr;

    auto child0_map = multimesh::same_simplex_dimension_surjection(parent, child0, {0});
    auto child1_map = multimesh::same_simplex_dimension_surjection(parent, child1, {0, 1});
    auto child2_map = multimesh::same_simplex_dimension_surjection(parent, child2, {0, 1, 2});

    parent.register_child_mesh(child0_ptr, child0_map);
    parent.register_child_mesh(child1_ptr, child1_map);
    parent.register_child_mesh(child2_ptr, child2_map);


    multimesh::MultiMeshSimplexVisitor print_type_visitor(PrintTypeSizeFunctor{});

    auto tups = parent.get_all(PrimitiveType::Triangle);
    for (const auto& t : tups) {
        print_type_visitor.execute_from_root(static_cast<TriMesh&>(parent), NavigatableSimplex(parent, PF, t));
    }

    logger().trace("edge visitor!");
    multimesh::MultiMeshSimplexVisitor print_edge_visitor(GetTypeSizeFunctorWithReturn{});


    for (const auto& t : tups) {
        print_edge_visitor.execute_from_root(parent, NavigatableSimplex(parent, PF, t));
        multimesh::MultiMeshSimplexEventVisitor print_edge_event_visitor(print_edge_visitor);
        print_edge_event_visitor.run_on_edges(PrintEdgeReturnsFunctor{});
    }
}
