#pragma once

#include <cstdint>

#include <array>
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



namespace detail {
int64_t swap_valence(const TriMesh& mesh, const Tuple& v, bool boundaries = true);
auto swap_valences(const TriMesh& mesh, const Tuple& t, bool boundaries=true) -> std::array<int64_t, 4>;
int64_t swap_valence_variance(const TriMesh& mesh, const Tuple& vertex);
// the variance of valences after a swap
int64_t swap_valence_variance_after(const TriMesh& mesh, const Tuple& vertex);

int64_t swap_valence_variance(const std::array<int64_t,4>& vertex_valences);
int64_t swap_valence_variance_after(const std::array<int64_t,4>& vertex_valences);
}


} // namespace wmtk::simplex
