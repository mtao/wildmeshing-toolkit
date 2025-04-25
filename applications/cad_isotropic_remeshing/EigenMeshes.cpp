#include "EigenMeshes.hpp"
#include <spdlog/spdlog.h>
#include <wmtk/Mesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/mesh_utils.hpp>


void EigenMeshes::compute_vf()
{
    VF.resize(V.M.rows());
    for (int j = 0; j < F.M.rows(); ++j) {
        for (const auto& vid : F.M.row(j)) {
            VF[vid].emplace(j);
        }
    }
}

EigenMeshesBuilder::EigenMeshesBuilder(const wmtk::Mesh& m, const std::string_view& pos_name)
    : mesh(m)
    , position_attribute_name(pos_name)
{}
EigenMeshes EigenMeshesBuilder::load(const std::vector<std::array<int64_t, 2>>& indices)
{
    assert(mesh.is_connectivity_valid());
    wmtk::utils::EigenMatrixWriter writer;
    writer.set_position_attribute_name(position_attribute_name);
    mesh.serialize(writer);

    EigenMeshes& em = meshes.emplace_back();
    em.V.m_start = total_V;
    em.F.m_start = total_F;


    writer.get_position_matrix(em.V.M);
    writer.get_FV_matrix(em.F.M);
    assert(em.F.M.maxCoeff() < em.V.M.rows());

    total_V = em.V.end();
    total_F = em.F.end();
    em.compute_vf();
    return em;
}
EigenMeshes EigenMeshesBuilder::merge_meshes()
{
    EigenMeshes R;
}

wmtk::Tuple EigenMeshes::update_to_fused(const wmtk::Tuple& t) const
{
    assert(t.global_cid() < F.M.rows());
    return wmtk::Tuple(t.local_vid(), t.local_eid(), t.local_fid(), t.global_cid() + F.start());
}
