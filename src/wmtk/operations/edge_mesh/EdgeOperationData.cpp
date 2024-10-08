#include "EdgeOperationData.hpp"

#include <wmtk/EdgeMesh.hpp>
namespace wmtk::operations::edge_mesh {
    std::vector<simplex::Simplex> EdgeOperationData::new_vertices(const Mesh& m) const {
        if(m.is_free()) {
            return {
                simplex_from_id(m,PrimitiveType::Vertex,m_free_split_v[0]),
                simplex_from_id(m,PrimitiveType::Vertex,m_free_split_v[1])
            };
        } else {
            return {
                simplex_from_id(m,PrimitiveType::Vertex,m_split_v)
            };

        }
    }
std::array<Tuple, 2> EdgeOperationData::input_endpoints(const EdgeMesh& m) const
{
    std::array<Tuple, 2> r;
    r[0] = m_operating_tuple;
    r[1] = m.switch_tuple(m_operating_tuple, PrimitiveType::Vertex);
    return r;
}

std::array<Tuple, 2> EdgeOperationData::split_output_edges(const EdgeMesh& m) const
{
    std::array<Tuple, 2> r;
    for (size_t j = 0; j < 2; ++j) {
        r[j] = tuple_from_id(m, PrimitiveType::Edge, m_split_e[j]);
    }
    return r;
}
} // namespace wmtk::operations::edge_mesh
