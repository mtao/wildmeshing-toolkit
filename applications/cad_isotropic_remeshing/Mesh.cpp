#include "Mesh.hpp"
#include <spdlog/spdlog.h>

#include <h5pp/h5pp.h>


void Mesh::load(h5pp::File& file, const std::filesystem::path& path)
{
    V = file.readDataset<Eigen::MatrixXd>((path / "V").string());
    F = file.readDataset<wmtk::MatrixXl>((path / "F").string());
}
void Topology::load(h5pp::File& file, const std::filesystem::path& path)
{
    {
        auto corners_path = (path / "corners");
        auto attr_names = file.getAttributeNames((corners_path).string());
        for (const auto& v : attr_names) {
            int64_t cid = std::stoi(v);
            int64_t vid = file.readAttribute<int64_t>((corners_path).string(), v);
            corner_to_vid[cid] = vid;
        }
    }
    {
        auto feature_edges_path = (path / "feature_edges");
        auto dsets = file.findDatasets("", (feature_edges_path).string(), -1, 0);
        for (const auto& v : dsets) {
            int64_t cid = std::stoi(v);
            auto vids = file.readDataset<std::vector<int64_t>>((feature_edges_path / v).string());
            feature_edge_to_vids_chain[cid] = vids;
        }
    }
    {
        auto patches_path = (path / "patches");
        auto dsets = file.findDatasets("", (patches_path).string(), -1, 0);
        for (const auto& v : dsets) {
            int64_t cid = std::stoi(v);
            spdlog::info("{} {}", patches_path.string(), v);
            auto vids = file.readDataset<std::vector<int64_t>>((patches_path / v).string());
            std::set<int64_t> vids2(vids.begin(), vids.end());
            patch_to_fids[cid] = vids2;
        }
    }
    // auto corners= file.findGroups("",corners_path.string(),-1,0);
    // for(const auto& g: corners) {
    // int64_t cid = std::stoi(g);
    //auto corner_path = corners_path / g;

    //}
}
void FusedOutput::load(h5pp::File& file, const std::filesystem::path& path)
{
    topology.load(file, path / "topology");
    mesh.load(file, path / "mesh");
}
