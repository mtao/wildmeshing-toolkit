#include "Mesh.hpp"
#include <spdlog/spdlog.h>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/internal/IndexSimplexMapper.hpp>


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
    assert(validate_features_end_in_corners());
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

bool Topology::validate_features_end_in_corners() const
{
    std::set<int64_t> corner_vids;
    std::transform(
        corner_to_vid.begin(),
        corner_to_vid.end(),
        std::inserter(corner_vids, corner_vids.end()),
        [](const auto& a) { return std::get<1>(a); });

    bool failed = false;
    for (const auto& [index, feature] : feature_edge_to_vids_chain) {
        if (!corner_vids.contains(feature.front())) {
            wmtk::logger().warn(
                "Chain {}'s first vertex was no a corner (vid={})",
                index,
                feature.front());
            failed = true;
        }
        if (!corner_vids.contains(feature.back())) {
            wmtk::logger().warn(
                "Chain {}'s last vertex was no a corner (vid={})",
                index,
                feature.back());
            failed = true;
        }
    }
    return !failed;
}

std::shared_ptr<wmtk::PointMesh> Topology::corner_mesh(wmtk::TriMesh& tri_mesh) const
{
    wmtk::utils::internal::IndexSimplexMapper ism(tri_mesh);
    std::vector<std::array<wmtk::Tuple, 2>> map;
    int64_t index = 0;
    for (const auto& [c, vid] : corner_to_vid) {
        map.emplace_back(std::array<wmtk::Tuple, 2>{
            {wmtk::Tuple(-1, -1, -1, index++),
             tri_mesh.get_tuple_from_id_simplex(
                 wmtk::simplex::IdSimplex(wmtk::PrimitiveType::Vertex, vid))}});
    }
    auto ret = std::make_shared<wmtk::PointMesh>(map.size());

    tri_mesh.register_child_mesh(ret, map);

    return ret;
}
std::shared_ptr<wmtk::EdgeMesh> Topology::feature_edge_mesh(wmtk::TriMesh& tri_mesh) const
{
    return nullptr;
}
