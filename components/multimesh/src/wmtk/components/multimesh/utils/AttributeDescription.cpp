#include "AttributeDescription.hpp"
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/components/multimesh/NamedMultiMesh.hpp>

#include <wmtk/components/utils/json_serialize_enum.hpp>
namespace wmtk::attribute {
// TODO: this definitely will cause a conflict someday if someone else wants to serialize
// attributetype
WMTK_NLOHMANN_JSON_SERIALIZE_ENUM(
    AttributeType,
    {
        {AttributeType::Char, "char"},
        {AttributeType::Double, "double"},
        {AttributeType::Int64, "int"},
        {AttributeType::Rational, "rational"},
    })
} // namespace wmtk::attribute

namespace wmtk::components::multimesh::utils {
auto AttributeDescription::operator<=>(const AttributeDescription&) const
    -> std::strong_ordering = default;
auto AttributeDescription::operator==(const AttributeDescription&) const -> bool = default;

namespace {
NLOHMANN_JSON_SERIALIZE_ENUM(
    PrimitiveType,
    {
        {PrimitiveType::Vertex, 0},
        {PrimitiveType::Edge, 1},
        {PrimitiveType::Triangle, 2},
        {PrimitiveType::Tetrahedron, 3},
    })
} // namespace
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(AttributeDescription)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(path);
    if (nlohmann_json_t.simplex_dimension.has_value()) {
        nlohmann_json_j["simplex_dimension"] = nlohmann_json_t.simplex_dimension.value();
    }
    if (nlohmann_json_t.type.has_value()) {
        nlohmann_json_j["type"] = nlohmann_json_t.type.value();
    }
    if (nlohmann_json_t.dimension.has_value()) {
        nlohmann_json_j["dimension"] = nlohmann_json_t.dimension.value();
    }
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(AttributeDescription)
{
    if (nlohmann_json_j.is_string()) {
        nlohmann_json_t.path = nlohmann_json_j.get<std::string>();
    } else {
        nlohmann_json_t.path = nlohmann_json_j["path"];
    }
    if (nlohmann_json_j.contains("simplex_dimension")) {
        nlohmann_json_t.simplex_dimension = nlohmann_json_j["simplex_dimension"];
    }
    if (nlohmann_json_j.contains("type")) {
        nlohmann_json_t.type = nlohmann_json_j["type"];
    }
    if (nlohmann_json_j.contains("dimension")) {
        nlohmann_json_t.dimension = nlohmann_json_j["dimension"];
    }
}

std::optional<PrimitiveType> AttributeDescription::primitive_type() const
{
    if (this->simplex_dimension.has_value()) {
        int8_t d = this->simplex_dimension.value();
        assert(d < 4);

        return get_primitive_type_from_id(d);
    } else {
        return {};
    }
}
AttributeDescription::AttributeDescription(
    const std::string_view& p,
    const wmtk::attribute::MeshAttributeHandle& mah)
    : path(p)
    , simplex_dimension(get_primitive_type_id(mah.primitive_type()))
    , type(mah.held_type())
{}
AttributeDescription::AttributeDescription(const wmtk::attribute::MeshAttributeHandle& mah)
    : AttributeDescription(mah.name(), mah)
{}
// AttributeDescription::AttributeDescription(const MeshCollection& mc, const
// wmtk::attribute::MeshAttributeHandle&mah): AttributeDescription(mc.get_path(mah), mah) {}
AttributeDescription::AttributeDescription(
    const NamedMultiMesh& mc,
    const wmtk::attribute::MeshAttributeHandle& mah)
    : AttributeDescription(mc.get_path(mah), mah)
{}

AttributeDescription::operator std::string() const
{
    auto get_tok = [](char c, const auto& opt) -> std::string {
        if (opt.has_value()) {
            if constexpr (std::is_same_v<
                              typename std::decay_t<decltype(opt)>::value_type,
                              attribute::AttributeType>) {
                if (opt.has_value()) {
                    return fmt::format(",{}={}", c, attribute::attribute_type_name(opt.value()));
                }
            } else {
                return fmt::format(",{}={}", c, opt.value());
            }
        }
        return "";
    };
    return fmt::format(
        "AttributeDescription({}t={},s={},d={})",
        path,
        get_tok('t', type),
        get_tok('s', simplex_dimension),
        get_tok('d', dimension));
}


bool AttributeDescription::fully_specified() const
{
    return simplex_dimension.has_value() && type.has_value() && dimension.has_value();
}
bool AttributeDescription::empty() const
{
    return path.empty() && !simplex_dimension.has_value() && !type.has_value() &&
           !dimension.has_value();
}
} // namespace wmtk::components::multimesh::utils
