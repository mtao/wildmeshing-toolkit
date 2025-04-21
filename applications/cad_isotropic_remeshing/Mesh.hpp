#pragma once
#include <Eigen/Dense>
#include <filesystem>
#include <map>
#include <set>
#include <wmtk/Types.hpp>

namespace h5pp {
class File;
}
namespace wmtk {
class PointMesh;
class EdgeMesh;
class TriMesh;
} // namespace wmtk

struct Mesh
{
    Eigen::MatrixXd V;
    wmtk::MatrixXl F;

    void load(h5pp::File& file, const std::filesystem::path& path);

    std::shared_ptr<wmtk::TriMesh> create() const;
};

struct Topology
{
    std::map<int64_t, std::set<int64_t>> patch_to_fids;
    std::map<int64_t, std::vector<int64_t>> feature_edge_to_vids_chain;
    std::map<int64_t, int64_t> corner_to_vid;
    void load(h5pp::File& file, const std::filesystem::path& path);

    std::shared_ptr<wmtk::PointMesh> corner_mesh(wmtk::TriMesh& tri_mesh) const;
    std::shared_ptr<wmtk::EdgeMesh> feature_edge_mesh(wmtk::TriMesh& tri_mesh) const;

    bool validate_features_end_in_corners() const;
};

struct FusedOutput
{
    Mesh mesh;
    Topology topology;

    void load(h5pp::File& file, const std::filesystem::path& path);
};
