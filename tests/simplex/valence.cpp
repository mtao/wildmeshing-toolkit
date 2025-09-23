#include <catch2/catch_test_macros.hpp>
#include <wmtk/Types.hpp>
#include <spdlog/spdlog.h>
#include <wmtk/simplex/valence.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>

#include "../tools/TriMesh_examples.hpp"
#include "../tools/TetMesh_examples.hpp"
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
using namespace wmtk;

TEST_CASE("tri_valences", "[simplex][valences]")
{
    {
        auto m = tests::single_triangle();

        Tuple v0(0,1,-1,0);
        Tuple v1(1,2,-1,0);
        Tuple v2(2,0,-1,0);

        CHECK(simplex::valence(m,v0) == 2);
        CHECK(simplex::valence(m,v1) == 2);
        CHECK(simplex::valence(m,v2) == 2);

    }
    {
        auto m = tests::quad();

        Tuple v0(0,2,-1,0);
        Tuple v1(1,2,-1,0);
        Tuple v2(2,0,-1,0);
        Tuple v3(0,1,-1,1);

        CHECK(simplex::valence(m,v0) == 3);
        CHECK(simplex::valence(m,v1) == 3);
        CHECK(simplex::valence(m,v2) == 2);
        CHECK(simplex::valence(m,v3) == 2);
        CHECK(simplex::detail::swap_valences(m,v0) == std::array<int64_t,4>{{5,5,4,4}});
        // 3 - 2 , 3 - 2 , 4 - 2 , 4 - 2
        // 1 , 1 , 2 , 2
        CHECK(simplex::detail::swap_valence_variance(m,v0) == 3);
        CHECK(simplex::detail::swap_valence_variance_after(m,v0) == 3);
    }
    {
        auto m = tests::two_neighbors_plus_one();

        Tuple v0(0,2,-1,0); 
        Tuple v1(1,2,-1,0);
        Tuple v2(2,0,-1,0);
        Tuple v3(0,1,-1,1);
        Tuple v4(2,1,-1,2);

        CHECK(simplex::valence(m,v0) == 4);
        CHECK(simplex::valence(m,v1) == 3);
        CHECK(simplex::valence(m,v2) == 3);
        CHECK(simplex::valence(m,v3) == 3);
        CHECK(simplex::valence(m,v4) == 3);
        CHECK_FALSE(m.is_boundary(PrimitiveType::Vertex, v0));
        CHECK(simplex::detail::swap_valence(m,v0) == 4);
        CHECK(m.is_boundary(PrimitiveType::Vertex, v1));
        CHECK(simplex::detail::swap_valence(m,v1) == 5);
        CHECK(m.is_boundary(PrimitiveType::Vertex, v2));
        CHECK(simplex::detail::swap_valence(m,v2) == 5);
        CHECK(m.is_boundary(PrimitiveType::Vertex, v3));
        CHECK(simplex::detail::swap_valence(m,v3) == 5);
        CHECK(m.is_boundary(PrimitiveType::Vertex, v4));
        CHECK(simplex::detail::swap_valence(m,v4) == 5);
        CHECK(simplex::detail::swap_valences(m,v0) == std::array<int64_t,4>{{4,5,5,5}});
        CHECK(simplex::detail::swap_valence_variance(m,v0) == 2+1);

        CHECK(simplex::detail::swap_valence_variance_after(m,v0) == 3+0);
        auto vs = operations::composite::TriEdgeSwap(m)(simplex::Simplex::edge(v0));
        REQUIRE(!vs.empty());
        REQUIRE(vs.size() == 1);
        CHECK(simplex::detail::swap_valence_variance(m,vs[0].tuple()) == 2+1);


        //CHECK(simplex::detail::swap_valence_variance(m,) == 3+0);
    }
}
