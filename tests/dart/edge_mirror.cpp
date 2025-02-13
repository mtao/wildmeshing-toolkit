#include <catch2/catch_test_macros.hpp>
#include <numeric>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/simplex_index_from_valid_index.hpp>

#include <wmtk/utils/primitive_range.hpp>
#include "tools/all_valid_local_tuples.hpp"
#include "tools/darts_using_faces.hpp"
using namespace wmtk;
using namespace wmtk::dart;

TEST_CASE("dart_edge_mirror", "[tuple]")
{
    // when other meshes are available add them here
    for (PrimitiveType mesh_type :
         {PrimitiveType::Edge, PrimitiveType::Triangle, PrimitiveType::Tetrahedron}) {
        dart::SimplexDart sd(mesh_type);
        assert(size_t(sd.valid_indices().size()) == sd.size());

        auto tuples = wmtk::tests::all_valid_local_tuples(mesh_type);

        for(const Tuple& t: tuples) {
        auto t = sd.dart_from_tuple(tuples[j])

        for (PrimitiveType pt : wmtk::utils::primitive_below(mesh_type)) {

        }
        }
}
