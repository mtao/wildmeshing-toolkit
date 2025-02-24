#include "from_boundary.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include "MeshCollection.hpp"
#include "from_tag.hpp"
#include "utils/decompose_attribute_path.hpp"


namespace wmtk::components::multimesh {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshBoundaryOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, boundary_dimension);
    if (!nlohmann_json_t.boundary_attribute.empty()) {
        WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(boundary_attribute, boundary_attribute_value);
    }
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshBoundaryOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(MultimeshBoundaryOptions)
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, boundary_dimension);
    if (!nlohmann_json_j.contains("boundary_attribute")) {
        WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(
            boundary_attribute,
            boundary_attribute_value);
    }
}
void MultimeshBoundaryOptions::to_json(nlohmann::json& j) const
{
    j = *this;
}
void MultimeshBoundaryOptions::from_json(const nlohmann::json& j)
{
    *this = j;
}


std::shared_ptr<Mesh> from_boundary(
    Mesh& mesh,
    const PrimitiveType ptype,
    const std::string& attribute_name,
    char value,
    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes)
{
    auto is_boundary_handle = mesh.register_attribute<char>(attribute_name, ptype, 1);
    auto is_boundary_accessor = mesh.create_accessor(is_boundary_handle.as<char>());

    for (const auto& t : mesh.get_all(ptype)) {
        is_boundary_accessor.scalar_attribute(t) = mesh.is_boundary(ptype, t) ? value : 0;
    }

    return from_tag(is_boundary_handle, value, passed_attributes);
}
void MultimeshBoundaryOptions::run(MeshCollection& mc) const
{
    auto& mesh = mc.get_mesh(mesh_path);
    std::string tag_name = "boundary_tag";

    if (!boundary_attribute.empty()) {
        auto [attr_mesh_path, attribute_name] =
            utils::decompose_attribute_path(boundary_attribute.path);

        assert(attr_mesh_path == mesh_path);
        if (boundary_attribute.simplex_dimension.has_value()) {
            assert(boundary_attribute.simplex_dimension.value() == boundary_dimension);
        }
        tag_name = attribute_name;
    }
    from_boundary(mesh, get_primitive_type_from_id(boundary_dimension), tag_name, {});
    if (boundary_attribute.empty()) {
    }
}

} // namespace wmtk::components::multimesh
