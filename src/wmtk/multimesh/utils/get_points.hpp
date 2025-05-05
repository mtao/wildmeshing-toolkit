#pragma once
#include <memory>
#include <vector>
#include <wmtk/Tuple.hpp>


namespace wmtk {
    class Mesh;
    class PointMesh;
}
namespace wmtk::multimesh::utils {
/**
 * @brief Extracts a child PointMesh from a sequence of vertex tuples
 *
 * @param mesh
 * @param point_tuples
 */
    std::shared_ptr<PointMesh> get_points(Mesh& m, const std::vector<Tuple>& point_tuples);
} // namespace wmtk::multimesh::utils
