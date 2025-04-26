#include "Mesh.hpp"
#include <spdlog/spdlog.h>
#include <wmtk/EdgeMesh.hpp>
#include <fstream>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/from_tag.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/internal/IndexSimplexMapper.hpp>
#include <wmtk/utils/mesh_utils.hpp>
#include "EigenMeshes.hpp"


#include <h5pp/h5pp.h>


void Mesh::load(h5pp::File& file, const std::filesystem::path& path)
{
    V = file.readDataset<Eigen::MatrixXd>((path / "V").string());
    F = file.readDataset<wmtk::MatrixXl>((path / "F").string());

    std::ofstream ofs("F.txt");
    ofs << F << std::endl;
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
std::shared_ptr<wmtk::TriMesh> Mesh::create() const
{
    auto m = std::make_shared<wmtk::TriMesh>();
    m->initialize(F);

    wmtk::mesh_utils::set_matrix_attribute(V, "vertices", wmtk::PrimitiveType::Vertex, *m);


    return m;
}

std::shared_ptr<wmtk::PointMesh> Topology::corner_mesh(wmtk::TriMesh& tri_mesh) const
{
    std::vector<std::array<wmtk::Tuple, 2>> map;

    auto tups = tri_mesh.get_all(wmtk::PrimitiveType::Vertex);
    int64_t index = 0;
    for (const auto& [c, vid] : corner_to_vid) {
        map.emplace_back(std::array<wmtk::Tuple, 2>{{wmtk::Tuple(-1, -1, -1, index++), tups[vid]}});
    }
    /*
    wmtk::utils::internal::IndexSimplexMapper ism(tri_mesh);
    for (const auto& [c, vid] : corner_to_vid) {
        map.emplace_back(std::array<wmtk::Tuple, 2>{
            {wmtk::Tuple(-1, -1, -1, index++),
             tri_mesh.get_tuple_from_id_simplex(
                 wmtk::simplex::IdSimplex(wmtk::PrimitiveType::Vertex, vid))}});
    }
    */
    auto ret = std::make_shared<wmtk::PointMesh>(map.size());

    tri_mesh.register_child_mesh(ret, map);

    return ret;
}
std::shared_ptr<wmtk::EdgeMesh> Topology::feature_edge_mesh(wmtk::TriMesh& tri_mesh) const
{
    EigenMeshesBuilder emb(tri_mesh, "vertices");

    auto attr = tri_mesh.register_attribute<char>("edge_tag", wmtk::PrimitiveType::Edge, 1);
    auto acc = attr.create_accessor<char, 1>();

    for (const auto& [key, chain] : feature_edge_to_vids_chain) {
        std::vector<std::array<int64_t, 2>> E;
        E.reserve(chain.size());
        for (size_t j = 0; j < chain.size() - 1; ++j) {
            E.emplace_back(std::array<int64_t, 2>{{chain[j], chain[j + 1]}});
            auto [a, b] = E.back();
            if (a == b) {
                // spdlog::warn("Chain {} had duplicate vertex at {} (skipping it)", key, j);
                E.pop_back();
            }
        }
        for (const auto& e : E) {
            acc.scalar_attribute(emb.edge_to_facet.at(e)) = 1;
        }


        // emb.load(E);
    }


    auto h = tri_mesh.get_attribute_handle<double>("vertices", wmtk::PrimitiveType::Vertex);
    auto p = std::dynamic_pointer_cast<wmtk::EdgeMesh>(
        wmtk::components::multimesh::from_tag(attr, 1, {h}));
    //p->delete_attribute(p->get_attribute_handle<char>("edge_tag", wmtk::PrimitiveType::Edge));
    tri_mesh.delete_attribute(tri_mesh.get_attribute_handle<char>("edge_tag", wmtk::PrimitiveType::Edge));
    return p;

    // auto em = emb.create_edge_mesh();

    // return em;
}
