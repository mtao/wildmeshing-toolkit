
#include "get_attribute.hpp"
#include <ranges>
#include <wmtk/Mesh.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "..//MeshCollection.hpp"
#include "../NamedMultiMesh.hpp"
#include "AttributeDescription.hpp"
#include "decompose_attribute_path.hpp"
#include "detail/attribute_ambiguous_error.hpp"
#include "detail/attribute_missing_error.hpp"
#include "detail/named_error_text.hpp"
#include "get_attribute.hpp"
#include "wmtk/utils/Logger.hpp"

namespace wmtk::components::multimesh::utils {
namespace detail {
template <typename T>
wmtk::attribute::MeshAttributeHandle
get_attribute(const Mesh& mesh, const std::string_view& name, PrimitiveType pt)
{
    if (mesh.has_attribute<T>(std::string(name), pt)) {
        return mesh.get_attribute_handle<T>(std::string(name), pt);
    } else {
        throw attribute_missing_error::make(
            std::string(name),
            wmtk::get_primitive_type_id(pt),
            wmtk::attribute ::attribute_type_enum_from_type<T>());
    }
}

wmtk::attribute::MeshAttributeHandle get_attribute(
    const Mesh& mesh,
    const std::string_view& name,
    PrimitiveType pt,
    wmtk::attribute::AttributeType type)
{
    using AT = wmtk::attribute::AttributeType;
    switch (type) {
#define ENTRY(TYPE)                                                                   \
    case TYPE:                                                                        \
        return get_attribute<wmtk::attribute::type_from_attribute_type_enum_t<TYPE>>( \
            mesh,                                                                     \
            name,                                                                     \
            pt);
        ENTRY(AT::Char);
        ENTRY(AT::Double);
        ENTRY(AT::Int64);
        ENTRY(AT::Rational);
#undef ENTRY
    default: assert(false);
    }
    return {};
}
std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const Mesh& mesh,
    const std::string_view& name,
    const std::optional<PrimitiveType>& pt,
    const std::optional<wmtk::attribute::AttributeType>& type,
    const std::optional<int64_t>& dimension)
{
    using AT = wmtk::attribute::AttributeType;
    // This order matches wmtk::components::utils::get_attributes
    constexpr static std::array<AT, 4> types{{AT::Char, AT::Int64, AT::Double, AT::Rational}};
    // constexpr static std::array<AT, 4> types{{AT::Int64, AT::Double, AT::Char, AT::Rational}};
    std::vector<AttributeDescription> possibilities;
    std::vector<wmtk::attribute::MeshAttributeHandle> ret;
    auto add_option = [&](PrimitiveType prim, AT t) {
        auto attr = get_attribute(mesh, name, prim, t);

        assert(attr.is_valid());
        uint8_t simplex_dimension = wmtk::get_primitive_type_id(prim);
        if (!dimension.has_value() || (attr.dimension() == dimension.value())) {
            ret.emplace_back(attr);
            possibilities.emplace_back(AttributeDescription{name, simplex_dimension, t});
        }
    };
    if (pt.has_value() && type.has_value()) {
        wmtk::logger().debug(
            "Reading attribute {} with pt {} and type {}",
            name,
            primitive_type_name(pt.value()),
            attribute_type_name(type.value()));
        add_option(pt.value(), type.value());

    } else if (pt.has_value()) {
        wmtk::logger().debug(
            "Reading attribute {} with pt {}",
            name,
            primitive_type_name(pt.value()));
        for (AT at : types) {
            try {
                wmtk::logger().trace(
                    "Attempting to read attribute {} with pt {} and guess {}",
                    name,
                    primitive_type_name(pt.value()),
                    attribute_type_name(at));
                add_option(pt.value(), at);
            } catch (const attribute_missing_error& e) {
                continue;
            }
        }
    } else if (type.has_value()) {
        wmtk::logger().debug(
            "Reading attribute {} with and type {}",
            name,
            attribute_type_name(type.value()));
        for (PrimitiveType p : wmtk::utils::primitive_below(mesh.top_simplex_type())) {
            try {
                wmtk::logger().trace(
                    "Attempting to read attribute {} with guess pt {} and type {}",
                    name,
                    primitive_type_name(p),
                    attribute_type_name(type.value()));
                add_option(p, type.value());
            } catch (const attribute_missing_error& e) {
                continue;
            }
        }
    } else {
        wmtk::logger().debug("Reading attribute {}", name);
        for (AT at : types) {
            for (PrimitiveType p : wmtk::utils::primitive_below(mesh.top_simplex_type())) {
                try {
                    wmtk::logger().trace(
                        "Attempting to read attribute {} with guess pt {} and guess type {}",
                        name,
                        primitive_type_name(p),
                        attribute_type_name(at));
                    add_option(p, at);
                } catch (const attribute_missing_error& e) {
                    continue;
                }
            }
        }
    }


    if (possibilities.empty()) {
        throw attribute_missing_error::make(name, pt, type);
    } else if (possibilities.size() > 1) {
        throw attribute_ambiguous_error::make(possibilities, name, pt, type);
    }

    return ret;
}
wmtk::attribute::MeshAttributeHandle get_attribute(
    const Mesh& mesh,
    const std::string_view& name,
    const std::optional<PrimitiveType>& pt,
    const std::optional<wmtk::attribute::AttributeType>& type,
    const std::optional<int64_t>& dimension)
{
    using AT = wmtk::attribute::AttributeType;
    // This order matches wmtk::components::utils::get_attributes
    constexpr static std::array<AT, 4> types{{AT::Char, AT::Int64, AT::Double, AT::Rational}};
    // constexpr static std::array<AT, 4> types{{AT::Int64, AT::Double, AT::Char, AT::Rational}};
    std::vector<AttributeDescription> possibilities;
    wmtk::attribute::MeshAttributeHandle ret;
    auto add_option = [&](PrimitiveType prim, AT t) {
        ret = get_attribute(mesh, name, prim, t);

        assert(ret.is_valid());
        uint8_t simplex_dimension = wmtk::get_primitive_type_id(prim);
        if (!dimension.has_value() || (ret.dimension() == dimension.value())) {
            possibilities.emplace_back(AttributeDescription{name, simplex_dimension, t});
        }
    };
    if (pt.has_value() && type.has_value()) {
        wmtk::logger().debug(
            "Reading attribute {} with pt {} and type {}",
            name,
            primitive_type_name(pt.value()),
            attribute_type_name(type.value()));
        add_option(pt.value(), type.value());

    } else if (pt.has_value()) {
        wmtk::logger().debug(
            "Reading attribute {} with pt {}",
            name,
            primitive_type_name(pt.value()));
        for (AT at : types) {
            try {
                wmtk::logger().trace(
                    "Attempting to read attribute {} with pt {} and guess {}",
                    name,
                    primitive_type_name(pt.value()),
                    attribute_type_name(at));
                add_option(pt.value(), at);
            } catch (const attribute_missing_error& e) {
                continue;
            }
        }
    } else if (type.has_value()) {
        wmtk::logger().debug(
            "Reading attribute {} with and type {}",
            name,
            attribute_type_name(type.value()));
        for (PrimitiveType p : wmtk::utils::primitive_below(mesh.top_simplex_type())) {
            try {
                wmtk::logger().trace(
                    "Attempting to read attribute {} with guess pt {} and type {}",
                    name,
                    primitive_type_name(p),
                    attribute_type_name(type.value()));
                add_option(p, type.value());
            } catch (const attribute_missing_error& e) {
                continue;
            }
        }
    } else {
        wmtk::logger().debug("Reading attribute {}", name);
        for (AT at : types) {
            for (PrimitiveType p : wmtk::utils::primitive_below(mesh.top_simplex_type())) {
                try {
                    wmtk::logger().trace(
                        "Attempting to read attribute {} with guess pt {} and guess type {}",
                        name,
                        primitive_type_name(p),
                        attribute_type_name(at));
                    add_option(p, at);
                } catch (const attribute_missing_error& e) {
                    continue;
                }
            }
        }
    }


    if (possibilities.empty()) {
        throw attribute_missing_error::make(name, pt, type);
    } else if (possibilities.size() > 1) {
        throw attribute_ambiguous_error::make(possibilities, name, pt, type);
    }

    assert(ret.is_valid());
    return ret;
}
} // namespace detail

wmtk::attribute::MeshAttributeHandle get_attribute(
    const NamedMultiMesh& mesh,
    const AttributeDescription& description)
{
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);
    return detail::get_attribute(
        mesh.get_mesh(mesh_path),
        attribute_name,
        description.primitive_type(),
        description.type,
        description.dimension);
}
wmtk::attribute::MeshAttributeHandle get_attribute(
    const MeshCollection& mesh,
    const AttributeDescription& description)
{
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);

    const Mesh& nmm = mesh.get_mesh(mesh_path);
    return detail::get_attribute(
        nmm,
        attribute_name,
        description.primitive_type(),
        description.type,
        description.dimension);
}

wmtk::attribute::MeshAttributeHandle get_attribute(
    const Mesh& mesh,
    const AttributeDescription& description)
{
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);
    return detail::get_attribute(
        mesh,
        attribute_name,
        description.primitive_type(),
        description.type,
        description.dimension);
}

std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const NamedMultiMesh& mesh,
    const AttributeDescription& description)
{
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);
    return detail::get_attributes(
        mesh.get_mesh(mesh_path),
        attribute_name,
        description.primitive_type(),
        description.type,
        description.dimension);
}
std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const MeshCollection& mesh,
    const AttributeDescription& description)
{
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);

    const Mesh& nmm = mesh.get_mesh(mesh_path);
    return detail::get_attributes(
        nmm,
        attribute_name,
        description.primitive_type(),
        description.type,
        description.dimension);
}

std::vector<wmtk::attribute::MeshAttributeHandle> get_attributes(
    const Mesh& mesh,
    const AttributeDescription& description)
{
    auto [mesh_path, attribute_name] = decompose_attribute_path(description);
    return detail::get_attributes(
        mesh,
        attribute_name,
        description.primitive_type(),
        description.type,
        description.dimension);
}

} // namespace wmtk::components::multimesh::utils
