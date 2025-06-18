#pragma once

#include "../tuple_map_attribute_io.hpp"

namespace wmtk {
class Mesh;
namespace dart {
template <int Dim, typename MeshType>
class DartAccessor;
}
} // namespace wmtk
namespace wmtk::multimesh::utils::internal {

void print_all_mapped_tuples(
    const dart::DartAccessor<1, Mesh>& a,
    const dart::DartAccessor<1, Mesh>& b);
void print_all_mapped_tuples(const Mesh& m);
} // namespace wmtk::multimesh::utils::internal
