#include "DEBUG_MultiMeshManager.hpp"
#include <catch2/catch_test_macros.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include "DEBUG_Mesh.hpp"

#include <wmtk/utils/Logger.hpp>


namespace wmtk::tests {
void DEBUG_MultiMeshManager::run_checks(const Mesh& m)
{
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
        check_child_map_valid(my_mesh, child_data);
    }
}
void DEBUG_MultiMeshManager::check_child_map_valid(const Mesh& my_mesh, const ChildData& child_data)
    const
{

    MultiMeshManager::check_child_map_valid(my_mesh, child_data);
}
} // namespace wmtk::tests
