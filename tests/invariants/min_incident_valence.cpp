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
#include <wmtk/multimesh/utils/extract_child_mesh_from_tag.hpp>
#include <wmtk/utils/orient.hpp>

using namespace wmtk;
using namespace wmtk::simplex;
using namespace wmtk::invariants;
using namespace wmtk::tests;

TEST_CASE("MinIncidentValenceInvariant", "[invariants][2D]")
{
    SECTION("single_triangle")
    {
        const DEBUG_TriMesh m = single_triangle();
        const MinIncidentValenceInvariant inv(m, 3);

        for (const Tuple& t : m.get_all(PrimitiveType::Edge)) {
            CHECK_FALSE(inv.before(Simplex::edge(m, t)));
            CHECK_FALSE(inv.after({}, {t}));
        }
    }
    SECTION("one_ear")
    {
        const DEBUG_TriMesh m = one_ear();
        const MinIncidentValenceInvariant inv(m, 3);

        const Simplex e_mid = Simplex::edge(m, m.edge_tuple_with_vs_and_t(0, 1, 0));

        for (const Tuple& t : m.get_all(PrimitiveType::Edge)) {
            const Simplex e = Simplex::edge(m, t);
            if (simplex::utils::SimplexComparisons::equal(m, e, e_mid)) {
                CHECK(inv.before(simplex::Simplex::edge(m, t)));
            } else {
                CHECK_FALSE(inv.before(simplex::Simplex::edge(m, t)));
            }
        }

        CHECK_FALSE(inv.after({}, m.get_all(PrimitiveType::Triangle)));
    }
    SECTION("edge_region")
    {
        const DEBUG_TriMesh m = edge_region();
        const MinIncidentValenceInvariant inv(m, 3);

        for (const Tuple& t : m.get_all(PrimitiveType::Edge)) {
            CHECK(inv.before(Simplex::edge(m, t)));
        }

        CHECK(inv.after({}, m.get_all(PrimitiveType::Triangle)));
    }
}
