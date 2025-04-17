#pragma once
#include <Eigen/Dense>
#include <wmtk/Types.hpp>
#include <map>
#include <set>
#include <filesystem>

namespace h5pp {
    class File;
}

struct Mesh {
    Eigen::MatrixXd V;
	wmtk::MatrixXl  F;

    void load( h5pp::File& file, const std::filesystem::path& path);
};

struct Topology {
	std::map<int64_t, std::set<int64_t>>  patch_to_fids;
	std::map<int64_t, std::vector<int64_t>> feature_edge_to_vids_chain;
	std::map<int64_t, int64_t> corner_to_vid;
    void load( h5pp::File& file, const std::filesystem::path& path);


};

struct FusedOutput {
	Mesh mesh;
	Topology topology;

    void load( h5pp::File& file, const std::filesystem::path& path);

};
