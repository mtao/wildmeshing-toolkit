

#include "create_attribute.hpp"
#include <ranges>
#include <wmtk/Mesh.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "..//MeshCollection.hpp"
#include "../NamedMultiMesh.hpp"
#include "AttributeDescription.hpp"
#include "create_attribute.hpp"
#include "decompose_attribute_path.hpp"
#include "detail/attribute_ambiguous_error.hpp"
#include "detail/attribute_missing_error.hpp"
#include "detail/named_error_text.hpp"
#include "wmtk/utils/Logger.hpp"

namespace wmtk::components::multimesh::utils {
namespace detail {
template <typename T>
wmtk::attribute::MeshAttributeHandle
create_attribute(Mesh& mesh, const std::string_view& name, PrimitiveType pt, int64_t dimension)
{
    return mesh.register_attribute<T>(std::string(name), pt, dimension);
}

wmtk::attribute::MeshAttributeHandle create_attribute(
    Mesh& mesh,
    const std::string_view& name,
    PrimitiveType pt,
    wmtk::attribute::AttributeType type,
    int64_t dim)
{
    using AT = wmtk::attribute::AttributeType;
    switch (type) {
#define ENTRY(TYPE)                                                                      \
    case TYPE:                                                                           \
        return create_attribute<wmtk::attribute::type_from_attribute_type_enum_t<TYPE>>( \
            mesh,                                                                        \
            name,                                                                        \
            pt,                                                                          \
            dim);
        ENTRY(AT::Char);
        ENTRY(AT::Double);
        ENTRY(AT::Int64);
        ENTRY(AT::Rational);
#undef ENTRY
    default: assert(false);
    }
    return {};
}
} // namespace detail

wmtk::attribute::MeshAttributeHandle create_attribute(
    NamedMultiMesh& mesh,
    const AttributeDescription& description)
{
    assert(description.fully_specified());
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);
    return detail::create_attribute(
        mesh.get_mesh(mesh_path),
        attribute_name,
        description.primitive_type().value(),
        description.type.value(),
        description.dimension.value());
}
wmtk::attribute::MeshAttributeHandle create_attribute(
    MeshCollection& mesh,
    const AttributeDescription& description)
{
    assert(description.fully_specified());
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);

    Mesh& nmm = mesh.get_mesh(mesh_path);
    return detail::create_attribute(
        nmm,
        attribute_name,
        description.primitive_type().value(),
        description.type.value(),
        description.dimension.value());
}

wmtk::attribute::MeshAttributeHandle create_attribute(
    Mesh& mesh,
    const AttributeDescription& description)
{
    assert(description.fully_specified());
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);
    return detail::create_attribute(
        mesh,
        attribute_name,
        description.primitive_type().value(),
        description.type.value(),
        description.dimension.value());
}

} // namespace wmtk::components::multimesh::utils
