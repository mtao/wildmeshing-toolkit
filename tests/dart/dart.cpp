#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <wmtk/autogen/utils/local_id_table_offset.hpp>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/simplex_index_from_permutation_index.hpp>

#include <wmtk/utils/primitive_range.hpp>
#include "tools/all_valid_local_tuples.hpp"
#include "tools/darts_using_faces.hpp"
using namespace wmtk;
using namespace wmtk::autogen;
using namespace wmtk::dart;
using namespace wmtk::tests;


TEST_CASE("tuple_autogen_permutation_indices_equal", "[tuple]")
{
    // when other meshes are available add them here
    for (PrimitiveType mesh_type :
         {PrimitiveType::Edge, PrimitiveType::Triangle, PrimitiveType::Tetrahedron}) {
        auto tuples = wmtk::tests::all_valid_local_tuples(mesh_type);
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_type);


        std::vector<int8_t> indices_from_tuples, permutation_indices_from_tuples;
        for (const auto& t : tuples) {
            indices_from_tuples.emplace_back(sd.permutation_index_from_tuple(t));
            permutation_indices_from_tuples.emplace_back(
                wmtk::autogen::utils::local_id_table_offset(mesh_type, t));
        }
        VectorX<int8_t> permutation_indices = sd.permutation_indices();
        std::vector<int8_t> range(permutation_indices.size());
        std::iota(range.begin(), range.end(), 0);
        CHECK(range == indices_from_tuples);

        std::vector<int8_t> permutation_indices_vec(
            permutation_indices.begin(),
            permutation_indices.end());

        CHECK(permutation_indices_vec == permutation_indices_from_tuples);
    }
}

TEST_CASE("tuple_autogen_index_dart_tuple_conversion", "[tuple]")
{
    // when other meshes are available add them here
    for (PrimitiveType mesh_type :
         {PrimitiveType::Edge, PrimitiveType::Triangle, PrimitiveType::Tetrahedron}) {
        auto tuples = wmtk::tests::all_valid_local_tuples(mesh_type);
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_type);

        for (const auto& t : tuples) {
            int8_t i = sd.permutation_index_from_tuple(t);
            Tuple nt = sd.tuple_from_permutation_index(0, i);

            for (PrimitiveType pt = PrimitiveType::Vertex; pt < mesh_type; pt = pt + 1) {
                CHECK(
                    sd.simplex_index(i, pt) ==
                    wmtk::dart::utils::simplex_index_from_permutation_index(mesh_type, i, pt));
                CHECK(sd.simplex_index(i, pt) == nt.local_id( pt));
            }


            CHECK(t == nt);
        }
    }
}

TEST_CASE("tuple_autogen_index_dart_group_structure", "[tuple]")
{
    // when other meshes are available add them here
    for (PrimitiveType mesh_type :
         {PrimitiveType::Edge, PrimitiveType::Triangle, PrimitiveType::Tetrahedron}) {
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_type);
        assert(size_t(sd.permutation_indices().size()) == sd.size());

        for (PrimitiveType pt : wmtk::utils::primitive_below(mesh_type)) {
            const int8_t index_switch = sd.primitive_as_index(pt);
            CHECK(sd.identity() == sd.product(index_switch, index_switch));
            CHECK(index_switch == sd.inverse(index_switch));
        }
        for (int8_t index = 0; index < sd.size(); ++index) {
            const int8_t inv = sd.inverse(index);
            CHECK(sd.product(index, inv) == sd.identity());
            CHECK(sd.product(inv, index) == sd.identity());
            // for (int8_t index2 = 0; index2 < sd.size(); ++index2) {
            //     const int8_t inv2 = sd.inverse(index2);
            //     const int8_t p = sd.product(index, index2);
            //     const int8_t pi = sd.product(inv2, inv);
            //     CHECK(pi == sd.inverse(p));
            //     CHECK(sd.product(p, pi) == sd.identity());
            //     CHECK(sd.product(pi, p) == sd.identity());
            // }
        }
    }
}
