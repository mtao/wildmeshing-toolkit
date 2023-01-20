#include <wmtk/utils/TransportablePoints.hpp>


using namespace wmtk;


TransportablePointsBase::~TransportablePointsBase() = default;

void TransportablePointsBase::before_hook(const TriMesh& m, const std::set<TriMesh::Tuple>& input_tris)
{
    std::set<size_t>& active_pts = active_point_bins.local();
    // go through set of input tris and try to move every point in an output tri
    for (size_t point_index = 0; point_index < triangle_indices.size(); ++point_index) {
        if (input_tris.find(point_index) != input_tris.end()) {
            active_pts.emplace(point_index);
            update_global_coordinate(m, point_index);
        }
    }
}

void TransportablePointsBase::after_hook(const TriMesh& m, const std::set<TriMesh::Tuple>& output_tris)
{
    // go through set of input tris and try to move every point in an output tri
    for (const size_t point_index : active_points.local()) {
        update_local_coordinate(m, point_index, output_tris);
    }
    active_point_bins.local().clear();
}


// derived class is required to store a global representation of the point, used in before_hook
void TransportablePointsBase::update_local_coordinate(
    const TriMesh& m,
    size_t point_index,
    const std::vector<std::array<double,3>>& possible_tris)
{
    // this point needs to be moved forward in this operation
    // try to see if it's in a triangle
    bool found = false;
    for (const size_t triangle_index : possible_tris) {
        if (point_in_triangle(m, triangle_index, point_index)) {
            triangle_indices[point_index] = triangle_index;
            barycentric_coordinates[point_index] = get_barycentric(m, triangle_index, point_index);
            found = true;
        }
    }

    if (!found) {
        spdlog::warn("Point not found in a triangle, backup mechanism required");
    }
}
