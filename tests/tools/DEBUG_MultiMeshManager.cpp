#include "DEBUG_MultiMeshManager.hpp"
#include <catch2/catch_test_macros.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/utils/internal/print_all_mapped_tuples.hpp>
#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include "DEBUG_Mesh.hpp"
#include <wmtk/multimesh/utils/check_map_valid.hpp>

#include <wmtk/utils/Logger.hpp>


namespace wmtk::tests {
void DEBUG_MultiMeshManager::run_checks(const Mesh& m)
{
    REQUIRE(wmtk::multimesh::utils::check_maps_valid(m));
    auto& mm = DEBUG_Mesh::multi_mesh_manager(m);
    mm.check_map_valid(m);
}

void DEBUG_MultiMeshManager::check_child_mesh_valid(const Mesh& my_mesh, const Mesh& child_mesh)
    const
{
    // TODO: implement this
}


void DEBUG_MultiMeshManager::check_map_valid(const Mesh& my_mesh) const
{
    for (int64_t index = 0; index < int64_t(children().size()); ++index) {
        const auto& child_data = children()[index];
        REQUIRE(bool(child_data.mesh));
        REQUIRE(child_data.mesh->absolute_multi_mesh_id().front() == index);
    }
    REQUIRE(wmtk::multimesh::utils::check_maps_valid(my_mesh));
}
void DEBUG_MultiMeshManager::check_child_map_valid(const Mesh& my_mesh, const ChildData& child_data)
    const
{
    // TODO: deprecate this away
    REQUIRE(wmtk::multimesh::utils::check_maps_valid(my_mesh));
    //MultiMeshManager::check_child_map_valid(my_mesh, child_data);
}

void DEBUG_MultiMeshManager::print(const Mesh& m) const
{
    for (const auto& c : children()) {
        auto [a, b] = get_map_const_accessors(m, c);
        spdlog::warn(
            "Map from [{}] to [{}]",
            fmt::join(m.absolute_multi_mesh_id(), ","),
            fmt::join(c.mesh->absolute_multi_mesh_id(), ","));

#if defined(WMTK_ENABLED_MULTIMESH_DART)
        wmtk::multimesh::utils::internal::print_all_mapped_tuples(a, b);
#endif
    }
}

} // namespace wmtk::tests
