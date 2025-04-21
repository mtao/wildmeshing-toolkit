#pragma once

#include <Eigen/Core>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Types.hpp>
namespace wmtk {
class EdgeMesh;
class TriMesh;
class TetMesh;

namespace dart {
class Dart;
}
} // namespace wmtk
namespace wmtk::dart::utils {


// finds the index of a permutation encoded by indices {0...N}
std::array<int64_t, 2> get_vertex_indices(const EdgeMesh&, PrimitiveType pt, const dart::Dart& d);
std::array<int64_t, 3> get_vertex_indices(const TriMesh&, PrimitiveType pt, const dart::Dart& d);
std::array<int64_t, 4> get_vertex_indices(const TetMesh&, PrimitiveType pt, const dart::Dart& d);
} // namespace wmtk::dart::utils
