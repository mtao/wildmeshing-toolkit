#include <wmtk/utils/TransportablePoints.hpp>


using namespace wmtk;


TransportablePointsBase::~TransportablePointsBase() = default;

void TransportablePointsBase::before_hook(const TriMesh& m, const std::vector<TriMesh::Tuple>& input_tris)
{
    std::vector<size_t> tids;
    std::transform(std::begin(input_tris),std::end(input_tris),std::back_inserter(tids), [&](const auto& t) { return t.fid(m); });
    spdlog::info("TransportablePointsBase::before_hook had tris {}", fmt::join(tids,","));
    std::set<size_t> input_tri_set;
    std::transform(input_tris.begin(),input_tris.end(), std::inserter(input_tri_set, input_tri_set.end()), [&](const TriMesh::Tuple& t) -> size_t { return t.fid(m); });

    std::set<size_t>& active_pts = active_point_bins.local();
    // go through set of input tris and try to move every point in an output tri
    for (size_t point_index = 0; point_index < triangle_tuples.size(); ++point_index) {
        if (input_tri_set.find(point_index) != input_tri_set.end()) {
            active_pts.emplace(point_index);
            update_global_coordinate(m, point_index);
        }
    }
}

void TransportablePointsBase::after_hook(const TriMesh& m, const std::vector<TriMesh::Tuple>& output_tris)
{
    std::vector<size_t> tids;
    std::transform(std::begin(output_tris),std::end(output_tris),std::back_inserter(tids), [&](const auto& t) { return t.fid(m); });
    spdlog::info("TransportablePointsBase::after_hook had tris {}", fmt::join(tids,","));
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
    const std::vector<TriMesh::Tuple>& possible_tris)
{
    // this point needs to be moved forward in this operation
    // try to see if it's in a triangle
    bool found = false;
    for (const TriMesh::Tuple& tup: possible_tris) {
        //const size_t triangle_index = tup.fid(m);
        if (point_in_triangle(m, tup, point_index)) {
            triangle_tuples[point_index] = tup;
            barycentric_coordinates[point_index] = get_barycentric(m, tup, point_index);
            found = true;
        }
    }

    if (!found) {
        spdlog::warn("Point not found in a triangle, backup mechanism required");
    }
}
