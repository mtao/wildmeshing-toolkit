#pragma once

#include <cstdint>

namespace wmtk {
class Mesh;
class TriMesh;
class TetMesh;
class Tuple;
namespace simplex {
class Simplex;
}
} // namespace wmtk
namespace wmtk::simplex {
int64_t valence(const Mesh& mesh, const Tuple& vertex);
int64_t valence(const TriMesh& mesh, const Tuple& vertex);
int64_t valence(const TetMesh& mesh, const Tuple& vertex);

int64_t split_valence_sum(const TriMesh& mesh, const Tuple& vertex);
// the sum of valences after a split
int64_t split_valence_sum_after(const TriMesh& mesh, const Tuple& vertex);


} // namespace wmtk::simplex
