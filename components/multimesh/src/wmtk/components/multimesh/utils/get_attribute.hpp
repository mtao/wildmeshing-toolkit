#pragma once
#include <nlohmann/json.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>

namespace wmtk {
class Mesh;
namespace components::multimesh {
class NamedMultiMesh;
class MeshCollection;
} // namespace components::multimesh
} // namespace wmtk

namespace wmtk::components::multimesh {
namespace utils {
struct AttributeDescription;
//
// desires json that has
// * name (str for attribute name)
// * mesh (mesh path, mesh only only overload does not use this)
// * type (str double/int/char/rational
// * simplex (int, dimension attribute belongs to
// if type and simplex are missing code will search in lexicographical of (primitive_type, type)
//    where double < int < char < rational for type
//
// this is somewhat redundant to wmtk::components::utils::get_attributes, but is designed around
// supporting multimesh paths.

wmtk::attribute::MeshAttributeHandle get_attribute(
    const NamedMultiMesh& mesh,
    const AttributeDescription& description);
wmtk::attribute::MeshAttributeHandle get_attribute(
    const MeshCollection& mesh,
    const AttributeDescription& description);
wmtk::attribute::MeshAttributeHandle get_attribute(
    const Mesh& mesh,
    const AttributeDescription& description);


std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const NamedMultiMesh& mesh,
    const AttributeDescription& description);
std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const MeshCollection& mesh,
    const AttributeDescription& description);
std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const Mesh& mesh,
    const AttributeDescription& description);

} // namespace utils
} // namespace wmtk::components::multimesh
