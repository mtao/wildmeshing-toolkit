#include "from_facet_bijection.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/same_simplex_dimension_bijection.hpp>
#include "MeshCollection.hpp"

namespace wmtk::components::multimesh {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshBijectionOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(parent_path, child_path);
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshBijectionOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(parent_path, child_path);
}
void MultimeshBijectionOptions::to_json(nlohmann::json& j) const
{
    j = *this;
}
void MultimeshBijectionOptions::from_json(const nlohmann::json& j)
{
    *this = j;
}

void from_facet_bijection(Mesh& parent, Mesh& child)
{
    auto child_map = wmtk::multimesh::same_simplex_dimension_bijection(parent, child);
    parent.register_child_mesh(child.shared_from_this(), child_map);
}

void MultimeshBijectionOptions::run(MeshCollection& mc) const
{
    auto& parent = mc.get_mesh(parent_path);
    auto& child = mc.get_mesh(child_path);
    assert(child.is_multi_mesh_root());
    from_facet_bijection(parent, child);
    auto& parent_nmm = mc.get_named_multimesh(parent_path);
    auto& child_nmm = mc.get_named_multimesh(child_path);
    assert(!child.is_multi_mesh_root());
    assert(child.is_from_same_multi_mesh_structure(parent));
    assert(child.absolute_multi_mesh_id() == std::vector<int64_t>{0});
    parent_nmm.append_child_mesh_names(parent, child_nmm);

}
} // namespace wmtk::components::multimesh
