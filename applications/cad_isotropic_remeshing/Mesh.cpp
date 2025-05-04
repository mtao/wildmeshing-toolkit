#include "Mesh.hpp"
#include <spdlog/spdlog.h>
#include <fstream>
#include <map>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/mesh_info/transfer/filtered_neighbor_count.hpp>
#include <wmtk/components/multimesh/from_tag.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/internal/IndexSimplexMapper.hpp>
#include <wmtk/utils/mesh_utils.hpp>
#include "EigenMeshes.hpp"


#include <h5pp/h5pp.h>


void Mesh::load(h5pp::File& file, const std::filesystem::path& path)
{
    V = file.readDataset<Eigen::MatrixXd>((path / "V").string());
    F = file.readDataset<wmtk::MatrixXl>((path / "F").string());

    assert(F.maxCoeff() < V.rows());
    assert(F.minCoeff() >= 0);
    std::ofstream ofs("F.txt");
    // ofs << F << std::endl;
}
void Topology::load(h5pp::File& file, const std::filesystem::path& path)
{
    // std::set<int64_t> special{55404, 55656, 55655, 57939};
    {
        auto corners_path = (path / "corners");
        auto attr_names = file.getAttributeNames((corners_path).string());
        for (const auto& v : attr_names) {
            int64_t cid = std::stoi(v);
            int64_t vid = file.readAttribute<int64_t>((corners_path).string(), v);
            corner_to_vid[cid] = vid;
            // if (special.find(vid) != special.end()) {
            //     spdlog::info("Corner {} with vid {} found", cid, vid);
            // }
        }
        spdlog::info("Obtained {} critical points", corner_to_vid.size());
    }
    {
        auto feature_edges_path = (path / "feature_edges");
        auto dsets = file.findDatasets("", (feature_edges_path).string(), -1, 0);
        for (const auto& v : dsets) {
            int64_t cid = std::stoi(v);
            auto vids = file.readDataset<std::vector<int64_t>>((feature_edges_path / v).string());
            feature_edge_to_vids_chain[cid] = vids;
            // for (const auto& vid : vids) {
            //     if (special.find(vid) != special.end()) {
            //         spdlog::info("feature edge {} with vids {} found", cid, vids);
            //         break;
            //     }
            // }
        }
        spdlog::info("Obtained {} feature edges", feature_edge_to_vids_chain.size());
    }
    {
        // std::set<int64_t>
        //     special_fids{102586, 106480, 112201, 112202, 105359, 105360, 110070, 113347};
        auto patches_path = (path / "patches");
        auto dsets = file.findDatasets("", (patches_path).string(), -1, 0);
        for (const auto& v : dsets) {
            int64_t cid = std::stoi(v);
            auto fids = file.readDataset<std::vector<int64_t>>((patches_path / v).string());
            std::set<int64_t> fids2(fids.begin(), fids.end());
            patch_to_fids[cid] = fids2;
            // std::set<int64_t> isect;
            // std::set_intersection(
            //     special_fids.begin(),
            //     special_fids.end(),
            //     fids2.begin(),
            //     fids2.end(),
            //     std::inserter(isect, isect.end()));
            // if (isect.size() > 0) {
            //     spdlog::info(
            //         "patch {} had {}, which overlaps {} baddies",
            //         cid,
            //         fmt::join(fids2, ","),
            //         fmt::join(isect, ","));
            // }
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
    assert(m->is_connectivity_valid());


    return m;
}
std::shared_ptr<wmtk::PointMesh> Topology::corner_mesh(
    const wmtk::attribute::MeshAttributeHandle& mah,
    wmtk::PrimitiveType pt,
    std::string_view tag_name)
{
    std::string tag_name2 = std::string(tag_name) + "_intermediate";
    assert(mah.held_type() == wmtk::attribute::AttributeType::Char);
    wmtk::attribute::MeshAttributeHandle mah2 = mah;

    // check for tags with more than N attributes with the value
    {
        wmtk::components::mesh_info::transfer::FilteredNeighborCount factory;
        factory.parameters = {{"simplex_dimension", 0}, {"under", 1}, {"over", 1}};
        auto attr = mah2.mesh().register_attribute<int64_t>(std::string(tag_name2), pt, 1);
        auto acc = attr.create_accessor<char, 1>();

        // using FT =
        // wmtk::components::mesh_info::transfer::FilteredNeighborCount::FunctorType<1,1,int64_t,int64_t>;
        // static_assert(FT::validInDim());
        // static_assert(FT::validOutDim());
        // static_assert(FT::validType());
        auto transfer = factory.create_T<1, 1, int64_t, char>(mah, attr);

        transfer->run_on_all();
    }

    {
        wmtk::components::mesh_info::transfer::Threshold factory;
        factory.parameters = {{"simplex_dimension", 0}, {"over", 2}};
        auto attr = mah2.mesh().register_attribute<int64_t>(std::string(tag_name), pt, 1);
        auto acc = attr.create_accessor<char, 1>();

        auto transfer = factory.create_T<1, 1, char, int64_t>(mah, attr);

        transfer->run_on_all();
        mah2 = attr;
    }
    auto h = mah.mesh().get_attribute_handle<double>("vertices", wmtk::PrimitiveType::Vertex);
    auto p = std::dynamic_pointer_cast<wmtk::PointMesh>(
        wmtk::components::multimesh::from_tag(mah2, 1, {h}));
    return p;

    /*
    wmtk::utils::EigenMatrixWriter em;
    edge_mesh.serialize(em);
    em.get_EV_matrix();
    std::vector<std::array<wmtk::Tuple, 2>> map;
        std::map<int64_t, std::set<int64_t>> edges;

        auto tups = edge_mesh.get_all(wmtk::PrimitiveType::Vertex);
        for (const auto& [key, chain] : feature_edge_to_vids_chain) {
            for (size_t j = 0; j < chain.size() - 1; ++j) {
                int64_t a = chain[j];
                int64_t b = chain[j + 1];
                if (a == b) {
                } else {
                    edges[a].emplace(b);
                    edges[b].emplace(a);
                }
            }


            // emb.load(E);
        }
        for (const auto& [key, values] : edges) {
            if (values.size() > 2) {
                bool identified_nonmanifold = false;
                for (const auto& [c, vid] : corner_to_vid) {
                    if (vid == key) {
                        identified_nonmanifold = true;
                    }
                }
                spdlog::warn("There was more nonmanifold than we expected");
            }
            map.emplace_back(
                std::array<wmtk::Tuple, 2>{{wmtk::Tuple(-1, -1, -1, map.size()), tups[key]}});
        }


    }

    auto ret = std::make_shared<wmtk::PointMesh>(map.size());

    tri_mesh.register_child_mesh(ret, map);

    return ret;
    */
}

std::shared_ptr<wmtk::PointMesh> Topology::corner_mesh(wmtk::TriMesh& tri_mesh) const
{
    std::vector<std::array<wmtk::Tuple, 2>> map;
    if (false) {
        std::map<int64_t, std::set<int64_t>> edges;

        auto tups = tri_mesh.get_all(wmtk::PrimitiveType::Vertex);
        for (const auto& [key, chain] : feature_edge_to_vids_chain) {
            for (size_t j = 0; j < chain.size() - 1; ++j) {
                int64_t a = chain[j];
                int64_t b = chain[j + 1];
                if (a == b) {
                } else {
                    edges[a].emplace(b);
                    edges[b].emplace(a);
                }
            }


            // emb.load(E);
        }
        bool overall_found_nonmanifold = false;
        for (const auto& [key, values] : edges) {
            if (values.size() > 2) {
                bool identified_nonmanifold = false;
                for (const auto& [c, vid] : corner_to_vid) {
                    if (vid == key) {
                        identified_nonmanifold = true;
                    }
                }
                if(!identified_nonmanifold) {
                    spdlog::warn("Vertex {} looks nonmanifold because it is attached to these vertices: {}",key ,fmt::join(values,","));
                    overall_found_nonmanifold = true;
                }
            }
            map.emplace_back(
                std::array<wmtk::Tuple, 2>{{wmtk::Tuple(-1, -1, -1, map.size()), tups[key]}});
        }


    }

    else {
        auto tups = tri_mesh.get_all(wmtk::PrimitiveType::Vertex);
        for (const auto& [c, vid] : corner_to_vid) {
            map.emplace_back(
                std::array<wmtk::Tuple, 2>{{wmtk::Tuple(-1, -1, -1, map.size()), tups[vid]}});
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
    }
    auto ret = std::make_shared<wmtk::PointMesh>(map.size());

    tri_mesh.register_child_mesh(ret, map);

    return ret;
}

std::shared_ptr<wmtk::EdgeMesh> Topology::feature_edge_mesh(wmtk::TriMesh& tri_mesh) const
{
    auto attr = add_feature_edge_mesh_tag(tri_mesh, "edge_tag");
    auto h = tri_mesh.get_attribute_handle<double>("vertices", wmtk::PrimitiveType::Vertex);
    auto p = std::dynamic_pointer_cast<wmtk::EdgeMesh>(
        wmtk::components::multimesh::from_tag(attr, 1, {h}));
    // p->delete_attribute(p->get_attribute_handle<char>("edge_tag",
    // wmtk::PrimitiveType::Edge));
    tri_mesh.delete_attribute(
        tri_mesh.get_attribute_handle<char>("edge_tag", wmtk::PrimitiveType::Edge));
    assert(p->is_connectivity_valid());
    return p;
}
wmtk::attribute::MeshAttributeHandle Topology::add_feature_edge_mesh_tag(
    wmtk::TriMesh& tri_mesh,
    std::string_view edge_name) const
{
    EigenMeshesBuilder emb(tri_mesh, "vertices");

    auto attr =
        tri_mesh.register_attribute<char>(std::string(edge_name), wmtk::PrimitiveType::Edge, 1);
    auto acc = attr.create_accessor<char, 1>();

    for (const auto& [key, chain] : feature_edge_to_vids_chain) {
        // spdlog::info("chain {}: {}", key, fmt::join(chain, ","));
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
    return attr;


    // auto em = emb.create_edge_mesh();

    // return em;
}
