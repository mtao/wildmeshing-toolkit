#pragma once

#include "LoggerDataCollector.h"
#include <spdlog/stopwatch.h>
#include "AdaptiveTessellation.h"
#include "wmtk/utils/getRSS.h"

namespace adaptive_tessellation {

class StatisticsObj
{
    double min_ = std::numeric_limits<double>::max();
    double max_ = std::numeric_limits<double>::lowest();
    double mean_ = std::numeric_limits<double>::lowest();
    double median_ = std::numeric_limits<double>::lowest();
    double std_dev_ = std::numeric_limits<double>::lowest();

public:
    StatisticsObj(std::vector<double> data)
    {
        std::sort(data.begin(), data.end());

        min_ = data[0];
        max_ = data[data.size() - 1];
        median_ = data[(data.size() - 1) / 2];

        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        mean_ = sum / data.size();

        std::vector<double> diff(data.size());
        std::transform(data.begin(), data.end(), diff.begin(), [this](double x) {
            return x - mean_;
        });
        const double sq_sum = std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
        std_dev_ = std::sqrt(sq_sum / data.size());
    }

    nlohmann::json to_json() const
    {
        // const nlohmann::json& js,
        return {
            {"min", min_},
            {"max", max_},
            {"mean", mean_},
            {"median", median_},
            {"std_dev", std_dev_}};
    }

    operator nlohmann::json() const { return to_json(); }
};


void LoggerDataCollector::log_json(
    const AdaptiveTessellation& mesh,
    const std::string& log_name,
    const bool with_vectors) const
{
    nlohmann::json info = general_info_to_json();
    if (with_vectors) {
        nlohmann::json vec_info = vectors_to_json();
        info.insert(vec_info.begin(), vec_info.end());
    }

    mesh.mesh_parameters.log({log_name, info});
}

void LoggerDataCollector::log_json_verbose(
    const AdaptiveTessellation& mesh,
    const std::string& log_name)
{
    log_json(mesh, log_name, true);
}


void LoggerDataCollector::evaluate_mesh(const AdaptiveTessellation& mesh)
{
    using Tuple = wmtk::TriMesh::Tuple;

    peak_memory_ = wmtk::getPeakRSS();
    // face stuff
    {
        const auto faces = mesh.get_faces();
        num_faces_ = faces.size();

        triangle_energies_.reserve(num_faces_);
        triangle_areas_.reserve(num_faces_);
        // triangle_min_angles.reserve(num_faces_);

        for (const Tuple& f : faces) {
            const auto vids = mesh.oriented_tri_vertices(f);
            Eigen::Matrix<double, 3, 2, Eigen::RowMajor> triangle;
            Eigen::Matrix<double, 3, 3, Eigen::RowMajor> triangle_displaced;
            for (size_t i = 0; i < 3; ++i) {
                triangle.row(i) = mesh.vertex_attrs[vids[i].vid(mesh)].pos;
                triangle_displaced.row(i) =
                    mesh.mesh_parameters.m_displacement->get(triangle(i, 0), triangle(i, 1));
            }
            Eigen::Matrix<double, 1, 1> double_area;
            igl::doublearea(triangle_displaced, Eigen::Matrix<int, 1, 3>{0, 1, 2}, double_area);
            const double triangle_area = std::sqrt(double_area(0, 0));
            const double triangle_energy =
                mesh.mesh_parameters.m_displacement->get_error_per_triangle(triangle);

            triangle_areas_.emplace_back(triangle_area);
            triangle_energies_.emplace_back(triangle_energy);
        }
    }
    // edge stuff
    {
        const auto edges = mesh.get_edges();

        edge_lengths_.reserve(edges.size());

        for (const Tuple& e : edges) {
            const double l = mesh.get_length3d(e);
            edge_lengths_.emplace_back(l);
        }
    }
    // vertex stuff
    {
        const auto vertices = mesh.get_vertices();
        num_vertices_ = vertices.size();
    }
}

nlohmann::json LoggerDataCollector::general_info_to_json() const
{
    const StatisticsObj edge_length_stats(edge_lengths_);
    const StatisticsObj energy_stats(triangle_energies_);
    const StatisticsObj area_stats(triangle_areas_);
    // const StatisticsObj min_angle_stats(triangle_min_angles_);

    return {
        {"peak_memory", peak_memory_},
        {"num_faces", num_faces_},
        {"num_vertices", num_vertices_},
        {"runtime", runtime_},
        {"edge_length_stats", edge_length_stats},
        {"triangle_energy_stats", energy_stats},
        {"triangle_area_stats", area_stats}};
}

nlohmann::json LoggerDataCollector::vectors_to_json() const
{
    return {
        {"edge_lengths", edge_lengths_},
        {"triangle_energies", triangle_energies_},
        {"triangle_areas", triangle_areas_}};
}
} // namespace adaptive_tessellation