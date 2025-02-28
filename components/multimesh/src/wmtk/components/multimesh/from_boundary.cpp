#include "from_boundary.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include "MeshCollection.hpp"
#include "from_tag.hpp"
#include "utils/decompose_attribute_path.hpp"


namespace wmtk::components::multimesh {
namespace {

attribute::MeshAttributeHandle fill_boundary_attribute(
    Mesh& mesh,
    const PrimitiveType ptype,
    const std::string& attribute_name,
    char value)
{
    auto is_boundary_handle = mesh.register_attribute<char>(attribute_name, ptype, 1);
    auto is_boundary_accessor = mesh.create_accessor(is_boundary_handle.as<char>());

    for (const auto& t : mesh.get_all(ptype)) {
        is_boundary_accessor.scalar_attribute(t) = mesh.is_boundary(ptype, t) ? value : 0;
    }
    return is_boundary_handle;
}

} // namespace

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshBoundaryOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, dimension, output_mesh_name);
    if (!nlohmann_json_t.attribute_name.empty()) {
        WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute_name, attribute_value);
    }
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshBoundaryOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(MultimeshBoundaryOptions)
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, dimension, output_mesh_name);
    if (nlohmann_json_j.contains("attribute_name")) {
        WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(attribute_name, attribute_value);
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
    auto is_boundary_handle = fill_boundary_attribute(mesh, ptype, attribute_name, value);
    return from_tag(is_boundary_handle, value, passed_attributes);
}
void MultimeshBoundaryOptions::run(MeshCollection& mc) const
{
    auto& mesh = mc.get_mesh(mesh_path);
    std::string tag_name = "_boundary_tag";
    char tag_value = 1;
    const auto& input_nmm = mc.get_named_multimesh(mesh_path);
    const PrimitiveType pt = get_primitive_type_from_id(dimension);


    if (!attribute_name.empty()) {
        tag_name = attribute_name;
        tag_value = attribute_value;
    }

    MultimeshTagOptions tag_opts;
    auto attr = fill_boundary_attribute(mesh, pt, tag_name, tag_value);
    tag_opts.output_mesh_name = output_mesh_name;
    tag_opts.tag_attribute = utils::AttributeDescription(input_nmm, attr);
    tag_opts.delete_tag_attribute = attribute_name.empty();
    tag_opts.value = tag_value;


    tag_opts.run(mc);
}

} // namespace wmtk::components::multimesh
