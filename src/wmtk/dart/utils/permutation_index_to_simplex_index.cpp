#include "permutation_index_to_simplex_index.hpp"
#include <wmtk/autogen/edge_mesh/simplex_index_from_permutation_index.hpp>
#include <wmtk/autogen/tet_mesh/simplex_index_from_permutation_index.hpp>
#include <wmtk/autogen/tri_mesh/simplex_index_from_permutation_index.hpp>

using namespace wmtk::autogen;
namespace wmtk::dart::utils {
int8_t permutation_index_to_simplex_index(
    PrimitiveType mesh_type,
    int8_t current_orientation,
    PrimitiveType target_type)
{
    switch (mesh_type) {
    case PrimitiveType::Edge:
        return edge_mesh::simplex_index_from_permutation_index(current_orientation, target_type);
    case PrimitiveType::Triangle:
        return tri_mesh::simplex_index_from_permutation_index(current_orientation, target_type);
    case PrimitiveType::Tetrahedron:
        return tet_mesh::simplex_index_from_permutation_index(current_orientation, target_type);
    case PrimitiveType::Vertex: return 0;
    default: assert(false);
    }
    return 0;
}

} // namespace wmtk::autogen::utils
