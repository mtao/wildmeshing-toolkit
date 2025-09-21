#include <catch2/catch_test_macros.hpp>

#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include <wmtk/simplex/utils/SimplexComparisons.hpp>
#include "tools/DEBUG_TetMesh.hpp"
#include "tools/DEBUG_TriMesh.hpp"
#include "tools/TetMesh_examples.hpp"
#include "tools/TriMesh_examples.hpp"

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/attribute/TypedAttributeHandle.hpp>
#include <wmtk/invariants/MinIncidentValenceInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/invariants/TetMeshSubstructureTopologyPreservingInvariant.hpp>
#include <wmtk/invariants/TriMeshSubstructureTopologyPreservingInvariant.hpp>
#include <wmtk/multimesh/utils/extract_child_mesh_from_tag.hpp>
#include <wmtk/utils/orient.hpp>

using namespace wmtk;
using namespace wmtk::simplex;
using namespace wmtk::invariants;
using namespace wmtk::tests;





TEST_CASE("tri_rational_inversion_invariant", "[invariants][2D]")
{
    DEBUG_TriMesh m = single_triangle();
    auto position_handle = m.register_attribute<Rational>("vertices", PrimitiveType::Vertex, 2);
    auto position_accessor = m.create_accessor<Rational>(position_handle);

    auto dposition_handle =
        m.register_attribute<double>("double_vertices", PrimitiveType::Vertex, 2);
    auto dposition_accessor = m.create_accessor<double>(dposition_handle);

    Tuple v0 = m.tuple_from_id(PrimitiveType::Vertex, 0);
    Tuple v1 = m.tuple_from_id(PrimitiveType::Vertex, 1);
    Tuple v2 = m.tuple_from_id(PrimitiveType::Vertex, 2);

    position_accessor.vector_attribute(v0) = Eigen::Vector2<Rational>(-1, -1);
    position_accessor.vector_attribute(v1) = Eigen::Vector2<Rational>(1, 1);
    position_accessor.vector_attribute(v2) = Eigen::Vector2<Rational>(1, -1);

    dposition_accessor.vector_attribute(v0) = Eigen::Vector2<double>(-1, -1);
    dposition_accessor.vector_attribute(v1) = Eigen::Vector2<double>(1, 1);
    dposition_accessor.vector_attribute(v2) = Eigen::Vector2<double>(1, -1);

    const invariants::SimplexInversionInvariant inv(m, position_handle.as<Rational>());
    const invariants::SimplexInversionInvariant dinv(m, dposition_handle.as<double>());
    Tuple t = v0;

    for (const auto& t : m.get_all(PrimitiveType::Edge)) {
        CHECK(inv.after({}, {t}) == dinv.after({}, {t}));
        CHECK(inv.after({}, {m.switch_vertex(t)}) == dinv.after({}, {m.switch_vertex(t)}));
        CHECK(inv.after({}, {m.switch_edge(t)}) == dinv.after({}, {m.switch_edge(t)}));
        CHECK(
            inv.after({}, {m.switch_vertex(m.switch_edge(t))}) ==
            dinv.after({}, {m.switch_vertex(m.switch_edge(t))}));
        CHECK(
            inv.after({}, {m.switch_edge(m.switch_vertex(t))}) ==
            dinv.after({}, {m.switch_edge(m.switch_vertex(t))}));
        CHECK(
            inv.after({}, {m.switch_vertex(m.switch_edge(m.switch_vertex(t)))}) ==
            dinv.after({}, {m.switch_vertex(m.switch_edge(m.switch_vertex(t)))}));
    }
}

TEST_CASE("orient2d", "[invariants][2D]")
{
    const Eigen::Vector2<Rational> v0 = Eigen::Vector2<Rational>(-1, -1);
    const Eigen::Vector2<Rational> v1 = Eigen::Vector2<Rational>(1, 1);
    const Eigen::Vector2<Rational> v2 = Eigen::Vector2<Rational>(1, -1);

    const Eigen::Vector2d v0d = v0.cast<double>();
    const Eigen::Vector2d v1d = v1.cast<double>();
    const Eigen::Vector2d v2d = v2.cast<double>();

    REQUIRE(wmtk::utils::wmtk_orient2d(v0, v1, v2) == wmtk::utils::wmtk_orient2d(v0d, v1d, v2d));
    REQUIRE(wmtk::utils::wmtk_orient2d(v0, v2, v1) == wmtk::utils::wmtk_orient2d(v0d, v2d, v1d));
}
