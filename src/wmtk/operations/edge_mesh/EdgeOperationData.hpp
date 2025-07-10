#pragma once
#include <array>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/operations/EdgeOperationData.hpp>

namespace wmtk {
class EdgeMesh;
}

namespace wmtk::operations::edge_mesh {
class EdgeOperationData : public wmtk::operations::EdgeOperationData
{
public:
    // EdgeOperationData() = default;
    using wmtk::operations::EdgeOperationData::EdgeOperationData;
    EdgeOperationData(EdgeOperationData&&) = default;
    EdgeOperationData& operator=(EdgeOperationData&&) = default;
    std::array<std::vector<int64_t>, 2> simplex_ids_to_delete;
    std::vector<int64_t> cell_ids_to_update_hash;


    std::array<int64_t, 2> m_split_e = std::array<int64_t, 2>{{-1, -1}};
    int64_t m_split_v = -1;

    std::array<int64_t, 2> m_free_split_v;

    std::array<Tuple, 2> input_endpoints(const EdgeMesh& m) const;
    std::array<Tuple, 2> split_output_edges(const EdgeMesh&) const;
    std::vector<simplex::Simplex> new_vertices(const Mesh& m) const;

    bool m_is_self_loop = false;
    // common simplicies
    std::array<int64_t, 2> m_neighbor_eids = {{-1, -1}};
};
} // namespace wmtk::operations::edge_mesh
