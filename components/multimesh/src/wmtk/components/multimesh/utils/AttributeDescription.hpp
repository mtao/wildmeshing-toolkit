#pragma once
#include <compare>
#include <optional>
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/attribute/AttributeType.hpp>
#include <wmtk/components/utils/json_macros.hpp>

namespace wmtk::attribute {
class MeshAttributeHandle;
WMTK_NLOHMANN_JSON_DECLARATION(AttributeType)
} // namespace wmtk::attribute

namespace wmtk::components::multimesh {
class MeshColleciton;
class NamedMultiMesh;
} // namespace wmtk::components::multimesh

namespace wmtk::components::multimesh::utils {


// the minimal information to uniquely extract an attribute handle
struct AttributeDescription
{
    std::string path;
    std::optional<uint8_t> simplex_dimension;
    std::optional<attribute::AttributeType> type;
    std::optional<int64_t> dimension;


    bool fully_specified() const;

    AttributeDescription() = default;
    AttributeDescription(const AttributeDescription&) = default;
    AttributeDescription(AttributeDescription&&) = default;
    AttributeDescription& operator=(const AttributeDescription&) = default;
    AttributeDescription& operator=(AttributeDescription&&) = default;
    ~AttributeDescription() = default;

    // path will just be the attribute name
    AttributeDescription(const wmtk::attribute::MeshAttributeHandle&);
    // path will be the longest multimesh name possible
    // AttributeDescription(const MeshCollection& mc, const wmtk::attribute::MeshAttributeHandle&);
    // a canonical per-path multimesh
    AttributeDescription(const NamedMultiMesh& mc, const wmtk::attribute::MeshAttributeHandle&);

    AttributeDescription(
        const std::string_view& p,
        const std::optional<uint8_t>& simp_dim,
        const std::optional<attribute::AttributeType>& t,
        const std::optional<int64_t>& dim = {})
        : path(p)
        , simplex_dimension(dim)
        , type(t)
        , dimension(dim)
    {}

    explicit AttributeDescription(
        const std::string_view& p,
        const std::optional<PrimitiveType>& pt,
        const std::optional<attribute::AttributeType>& t,
        const std::optional<int64_t>& dim = {})

        : AttributeDescription(
              p,
              pt.has_value() ? std::optional<uint8_t>{wmtk::get_primitive_type_id(pt.value())}
                             : std::optional<uint8_t>{},
              t,
              dim)
    {}
    AttributeDescription(const std::string_view& str)
        : AttributeDescription{str, std::optional<uint8_t>{}, {}}
    {}


    std::optional<PrimitiveType> primitive_type() const;

    auto operator<=>(const AttributeDescription&) const -> std::strong_ordering;
    auto operator==(const AttributeDescription&) const -> bool;
    operator std::string() const;


    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeDescription)
private:
    // helper constructor so we can override the path while still reading off other properties from
    // the MAH
    AttributeDescription(const std::string_view& p, const wmtk::attribute::MeshAttributeHandle&);
};
} // namespace wmtk::components::multimesh::utils
