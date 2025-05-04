#include "get_points.hpp"
#include <wmtk/PointMesh.hpp>
namespace wmtk::multimesh::utils {
/**
 * @brief Extracts a child mesh from a sequence of vertex tuples
 *
 * @param mesh
 * @param point_tuples
 */
std::shared_ptr<PointMesh> get_points(Mesh& m, const std::vector<Tuple>& point_tuples)
{
    std::vector<std::array<wmtk::Tuple, 2>> map;
    for (size_t j = 0; j < point_tuples.size(); ++j) {
        map.emplace_back(std::array<wmtk::Tuple, 2>{{wmtk::Tuple(-1, -1, -1, j), point_tuples[j]}});
    }

    auto ret = std::make_shared<wmtk::PointMesh>(point_tuples.size());

    m.register_child_mesh(ret, map);
    return ret;
}
} // namespace wmtk::multimesh::utils
