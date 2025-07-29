#include "consolidate.hpp"
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/MultiMeshVisitor.hpp>


namespace wmtk::multimesh {
void consolidate(Mesh& mesh)
{
    std::map<
        std::vector<int64_t>,
        std::tuple<std::vector<std::vector<int64_t>>, std::vector<std::vector<int64_t>>>>
        update_maps;

    {
        auto run = [&](auto&& m) {
            if constexpr (!std::is_const_v<std::remove_reference_t<decltype(m)>>) {
                update_maps[m.absolute_multi_mesh_id()] = m.consolidate_update_data();
            }
        };
        multimesh::MultiMeshVisitor visitor(run);
        visitor.execute_from_root(mesh);
    }
    {
        auto run = [&](auto&& m) {
            if constexpr (!std::is_const_v<std::remove_reference_t<decltype(m)>>) {
                const auto& pr = update_maps.at(m.absolute_multi_mesh_id());
                m.consolidate_maps(pr);
            }
        };
        multimesh::MultiMeshVisitor visitor(run);
        visitor.execute_from_root(mesh);
    }
    {
        auto run = [&](auto&& m) {
            if constexpr (!std::is_const_v<std::remove_reference_t<decltype(m)>>) {
                const auto& pr = update_maps.at(m.absolute_multi_mesh_id());
                m.consolidate_attributes(pr);
                m.consolidate_connectivity(pr);
            }
        };
        multimesh::MultiMeshVisitor visitor(run);
        visitor.execute_from_root(mesh);
    }
}
} // namespace wmtk::multimesh
