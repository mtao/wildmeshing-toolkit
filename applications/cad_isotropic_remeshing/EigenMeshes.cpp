#include "EigenMeshes.hpp"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/utils/DisjointSet.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/mesh_utils.hpp>


void EigenMeshes::compute_dual()
{
    VF.resize(V.M.rows());
    for (int j = 0; j < F.M.rows(); ++j) {
        for (const auto& vid : F.M.row(j)) {
            VF[vid].emplace(j);
        }
    }
}

EigenMeshesBuilder::EigenMeshesBuilder(wmtk::Mesh& m, const std::string_view& pos_name)
    : mesh(m)
    , position_attribute_name(pos_name)
{
    assert(mesh.is_connectivity_valid());
    wmtk::utils::EigenMatrixWriter writer;
    writer.set_position_attribute_name(position_attribute_name);
    mesh.serialize(writer);

    writer.get_position_matrix(base_mesh.V.M);
    switch (m.top_simplex_type()) {
    default:
    case wmtk::PrimitiveType::Vertex: break;
    case wmtk::PrimitiveType::Edge: writer.get_EV_matrix(base_mesh.F.M); break;
    case wmtk::PrimitiveType::Triangle: writer.get_FV_matrix(base_mesh.F.M); break;
    case wmtk::PrimitiveType::Tetrahedron: writer.get_TV_matrix(base_mesh.F.M);
    }

    if (true) {
        std::ofstream ofs("F2.txt");
        ofs << base_mesh.F.M << std::endl;
    }

    auto domain_tups = mesh.get_all(wmtk::PrimitiveType::Edge);
    for (const auto& t : domain_tups) {
        int64_t a = mesh.id(t, wmtk::PrimitiveType::Vertex);
        wmtk::Tuple t2 = mesh.switch_tuple(t, wmtk::PrimitiveType::Vertex);
        int64_t b = mesh.id(t2, wmtk::PrimitiveType::Vertex);

        // spdlog::warn("Mapping trimesh {},{} to tuple {}", a, b, std::string(t));
        //  if (a <= b)
        {
            std::array<int64_t, 2> x{{a, b}};
            edge_to_facet.emplace(x, t);
        }
        // else
        {
            std::array<int64_t, 2> x{{b, a}};
            edge_to_facet.emplace(x, t2);
        }
    }
}
const EigenMeshes& EigenMeshesBuilder::load(const std::vector<std::array<int64_t, 2>>& indices)
{
    EigenMeshes& em = meshes.emplace_back();
    em.V.m_start = total_V;
    em.F.m_start = total_F;

    std::map<int64_t, int64_t> indexer;
    std::vector<wmtk::Tuple> my_tuples;
    my_tuples.reserve(indices.size());

    // spdlog::info("{}", fmt::join(indices, ","));

    for (const auto& a : indices) {
        std::array<int64_t, 2> x = a;
        // std::sort(x.begin(), x.end());
        wmtk::Tuple t = edge_to_facet.at(x);
        if (x[0] != a[0]) {
            t = mesh.switch_tuple(t, wmtk::PrimitiveType::Vertex);
        }
        spdlog::info("{}", std::string(t));
        my_tuples.emplace_back(t);

        for (const int64_t b : a) {
            int64_t s = indexer.size();
            indexer.try_emplace(b, s);
        }
    }
    tuples.emplace_back(my_tuples);
    auto& F = em.F.M;
    auto& V = em.V.M;
    F.resize(indices.size(), 2);
    V.resize(indexer.size(), 3);

    {
        {
            for (size_t j = 0; j < indices.size(); ++j) {
                for (size_t k = 0; k < 2; ++k) {
                    F(j, k) = indexer.at(indices[j][k]);
                }
            }
        }
    }


    assert(em.F.M.maxCoeff() < em.V.M.rows());

    total_V = em.V.end();
    total_F = em.F.end();
    em.compute_dual();
    return em;
}
EigenMeshes EigenMeshesBuilder::merge_meshes() const
{
    EigenMeshes R;
    auto& V = R.V.M;
    auto& F = R.F.M;

    V.resize(total_V, meshes[0].V.M.cols());
    F.resize(total_F, meshes[0].F.M.cols());
    F.setConstant(-1);
    V.setConstant(-1);
    for (auto& em : meshes) {
        em.F.assign(F, em.V.start());
        em.V.assign(V);
        // patch_labels.segment(em.F.start(),
        // em.F.M.rows()).setConstant(name_to_patch_ids.at(name));
    }
    return R;
}

wmtk::Tuple EigenMeshes::update_to_fused(const wmtk::Tuple& t) const
{
    assert(t.global_cid() < F.M.rows());
    return wmtk::Tuple(t.local_vid(), t.local_eid(), t.local_fid(), t.global_cid() + F.start());
}

std::shared_ptr<wmtk::EdgeMesh> EigenMeshesBuilder::create_edge_mesh()
{
    auto em = std::make_shared<wmtk::EdgeMesh>();

    EigenMeshes m = merge_meshes();
    // std::cout << m.F.M << std::endl;
    em->initialize(m.F.M);

    auto domain_tups = em->get_all(wmtk::PrimitiveType::Edge);

    std::vector<std::array<wmtk::Tuple, 2>> tups;
    tups.reserve(domain_tups.size());

    size_t index = 0;
    for (const auto& tt : tuples) {
        for (const auto& t : tt) {
            tups.emplace_back(std::array<wmtk::Tuple, 2>{{domain_tups[index++], t}});
            const auto& [a, b] = tups.back();
            spdlog::info("{} => {}", std::string(a), std::string(b));
        }
    }

    mesh.register_child_mesh(em, tups);


    return em;
}
