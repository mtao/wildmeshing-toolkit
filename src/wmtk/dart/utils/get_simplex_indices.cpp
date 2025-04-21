#include "get_simplex_indices.hpp"

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/utils/get_canonical_simplex.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
namespace wmtk::dart::utils {

std::array<int64_t, 2> get_vertex_indices(const EdgeMesh& m, PrimitiveType pt, const dart::Dart& d)
{
    return 0;
}
std::array<int64_t, 3> get_vertex_indices(const TriMesh& m, PrimitiveType pt, const dart::Dart& d)
{
    return 0;
}
std::array<int64_t, 4> get_vertex_indices(const TetMesh& m, PrimitiveType pt, const dart::Dart& d)
{
    return 0;
}


} // namespace wmtk::dart::utils
