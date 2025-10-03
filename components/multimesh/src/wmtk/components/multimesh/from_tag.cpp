#include "from_tag.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/components/configurator/Configurator.hpp>
#include <wmtk/components/configurator/transfer/TransferStrategyFactoryCollection.hpp>
#include <wmtk/components/configurator/transfer/TransferStrategyOptions.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>
#include <wmtk/multimesh/utils/extract_child_mesh_from_tag.hpp>
#include <wmtk/multimesh/utils/transfer_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>
#include <wmtk/utils/internal/is_manifold.hpp>
#include "MeshCollection.hpp"
#include "MultimeshRunnableOptions.hpp"
#include "utils/get_attribute.hpp"
#include "wmtk/components/utils/json_macros.hpp"


namespace wmtk::components::multimesh {

namespace {

std::shared_ptr<Mesh> from_tag(
    wmtk::attribute::MeshAttributeHandle& handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,
    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes,
    bool manifold_decomposition)
{
    auto child_mesh = wmtk::multimesh::utils::extract_and_register_child_mesh_from_tag(
        handle,
        tag_value,
        false,
        manifold_decomposition);

    for (const auto& attr : passed_attributes) {
        assert(attr.is_same_mesh(handle.mesh()));

        wmtk::multimesh::utils::transfer_attribute(attr, *child_mesh);
    }

    return child_mesh;
}
} // namespace

auto from_tag(const FromTagOptions& options) -> std::shared_ptr<Mesh>
{
    // constness is annoying, but want to let rvalues get passed in?
    wmtk::attribute::MeshAttributeHandle h = options.mesh.handle;
    return from_tag(
        h,
        options.mesh.value,
        options.passed_attributes,
        options.manifold_decomposition);
}
auto from_tag(
    const wmtk::attribute::MeshAttributeHandle& handle,

    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,

    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes,
    bool manifold_decomposition) -> std::shared_ptr<Mesh>

{
    FromTagOptions opts;
    opts.mesh = {handle, tag_value};
    opts.passed_attributes = passed_attributes;
    opts.manifold_decomposition = manifold_decomposition;
    return from_tag(opts);
}

MultimeshTagOptions::MultimeshTagOptions() = default;
MultimeshTagOptions::~MultimeshTagOptions() = default;
MultimeshTagOptions::MultimeshTagOptions(MultimeshTagOptions&&) = default;
MultimeshTagOptions& MultimeshTagOptions::operator=(MultimeshTagOptions&&) = default;
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
    opts.manifold_decomposition = manifold_decomposition;
    for (const auto& ad : passed_attributes) {
        opts.passed_attributes.emplace_back(utils::get_attribute(mc, tag_attribute));
    }

    return opts;
}
void MultimeshTagOptions::run(MeshCollection& mc) const
{
    std::vector<attribute::MeshAttributeHandle> creation_attr_handles;
    if (!creation_attributes.empty()) {
        configurator::Configurator configurator(mc);
        for (const auto& [name, attr] : creation_attributes) {
            auto t = configurator.create_transfer_strategy(name, attr);
            t->run_on_all();
            creation_attr_handles.emplace_back(t->handle());
        }
        auto mah = utils::get_attribute(mc, tag_attribute);
        if (delete_tag_attribute) {
            for (auto& h : creation_attr_handles) {
                if (mah == h) {
                    continue;
                }
                assert(h.exists());
                h.mesh().delete_attribute(h);
            }
        }
    }
    auto tag_opts = toTagOptions(mc);
    auto mptr = from_tag(tag_opts);
    auto& nmm = mc.get_named_multimesh(tag_attribute.path);
    nmm.set_name(*mptr, output_mesh_name);

    if (delete_tag_attribute) {
        if (tag_opts.mesh.handle.exists()) {
            tag_opts.mesh.handle.mesh().delete_attribute(tag_opts.mesh.handle);
        }
    }
}
// bool MultimeshTagOptions::operator==(const MultimeshTagOptions&) const = default;


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshTagOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(
        tag_attribute,
        output_mesh_name,
        delete_tag_attribute,
        manifold_decomposition,
        creation_attributes);

    std::visit(
        [&](const auto& v) noexcept {
            if constexpr (std::is_same_v<std::decay_t<decltype(v)>, wmtk::Rational>) {
            } else {
                nlohmann_json_j["value"] = v;
            }
        },
        nlohmann_json_t.value);
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshTagOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(MultimeshTagOptions)
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(tag_attribute, output_mesh_name);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(
        manifold_decomposition,
        delete_tag_attribute,
        creation_attributes);
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
