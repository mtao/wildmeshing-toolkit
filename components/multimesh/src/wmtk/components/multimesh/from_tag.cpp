#include "from_tag.hpp"
#include <spdlog/spdlog.h>
#include <wmtk/Mesh.hpp>
#include <wmtk/multimesh/utils/extract_child_mesh_from_tag.hpp>
#include <wmtk/multimesh/utils/transfer_attribute.hpp>
#include "MeshCollection.hpp"
#include "MultimeshRunnableOptions.hpp"
#include "utils/get_attribute.hpp"


namespace wmtk::components::multimesh {

namespace {

std::shared_ptr<Mesh> from_tag(
    wmtk::attribute::MeshAttributeHandle& handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,
    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes)
{
    auto child_mesh =
        wmtk::multimesh::utils::extract_and_register_child_mesh_from_tag(handle, tag_value);

    for (const auto& attr : passed_attributes) {
        assert(attr.is_same_mesh(handle.mesh()));

        wmtk::multimesh::utils::transfer_attribute(attr, *child_mesh);
    }
    return child_mesh;
}
} // namespace

std::shared_ptr<Mesh> from_tag(const FromTagOptions& options)
{
    // constness is annoying, but want to let rvalues get passed in?
    wmtk::attribute::MeshAttributeHandle h = options.mesh.handle;
    return from_tag(h, options.mesh.value, options.passed_attributes);
}
std::shared_ptr<Mesh> from_tag(
    const wmtk::attribute::MeshAttributeHandle& handle,

    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,

    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes)

{
    FromTagOptions opts;
    opts.mesh = {handle, tag_value};
    opts.passed_attributes = passed_attributes;
    return from_tag(opts);
}

MultimeshTagOptions::~MultimeshTagOptions() = default;
FromTagOptions MultimeshTagOptions::toTagOptions(const MeshCollection& mc) const
{
    auto mah = utils::get_attribute(mc, tag_attribute);
    FromTagOptions opts;
    if (tag_attribute.type.has_value()) {
        auto child_at = std::visit(
            [](const auto& v) -> wmtk::attribute::AttributeType {
                return wmtk::attribute::attribute_type_enum_from_type<std::decay_t<decltype(v)>>();
                //
            },
            value);
        assert(tag_attribute.type.value() == child_at);
    }
    opts.mesh = TaggedRegion{mah, value};

    return opts;
}
void MultimeshTagOptions::run(MeshCollection& mc) const
{
    auto tag_opts = toTagOptions(mc);
    auto mptr = from_tag(tag_opts);
    auto& nmm = mc.get_named_multimesh(tag_attribute.path);
    nmm.set_name(*mptr, output_mesh_name);
    if (delete_tag_attribute) {
        tag_opts.mesh.handle.mesh().delete_attribute(tag_opts.mesh.handle);
    }
}
bool MultimeshTagOptions::operator==(const MultimeshTagOptions&) const = default;


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshTagOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(tag_attribute, output_mesh_name, delete_tag_attribute);

    std::visit(
        [&](const auto& v) {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, wmtk::Rational>) {
            } else {
                nlohmann_json_j["value"] = v;
            }
        },
        nlohmann_json_t.value);
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshTagOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(tag_attribute, output_mesh_name);

    if (nlohmann_json_j.contains("delete_tag_attribute")) {
        nlohmann_json_t.delete_tag_attribute = nlohmann_json_j["delete_tag_attribute"];
    } else {
        nlohmann_json_t.delete_tag_attribute = false;
    }
    const auto& type_opt = nlohmann_json_t.tag_attribute.type;
    attribute::AttributeType type;
    if (type_opt.has_value()) {
        type = *type_opt;
    } else {
        type = attribute::AttributeType::Char;
    }
    auto& v = nlohmann_json_j["value"];
    switch (type) {
    case attribute::AttributeType::Char: nlohmann_json_t.value = (v.get<char>()); break;
    case attribute::AttributeType::Int64: nlohmann_json_t.value = (v.get<int64_t>()); break;
    case attribute::AttributeType::Double: nlohmann_json_t.value = (v.get<double>()); break;
    case attribute::AttributeType::Rational:
    default: assert(false);
    }
}
void MultimeshTagOptions::to_json(nlohmann::json& j) const
{
    j = *this;
}
void MultimeshTagOptions::from_json(const nlohmann::json& j)
{
    *this = j;
}
} // namespace wmtk::components::multimesh
