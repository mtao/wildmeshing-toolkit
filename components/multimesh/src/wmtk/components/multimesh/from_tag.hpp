#pragma once
#include "MultimeshRunnableOptions.hpp"
#include "utils/AttributeDescription.hpp"

#include <memory>
#include <optional>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/components/utils/json_macros.hpp>

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;
}
} // namespace wmtk
namespace wmtk::components::multimesh {
class MeshCollection;


struct TaggedRegion
{
    wmtk::attribute::MeshAttributeHandle handle;
    // tagged region is where == this value
    wmtk::attribute::MeshAttributeHandle::ValueVariant value;
};
struct FromTagOptions
{
    TaggedRegion mesh;
    std::optional<TaggedRegion> boundary;
    std::vector<wmtk::attribute::MeshAttributeHandle> passed_attributes;
};

// json serializable version
struct MultimeshTagOptions : public MultimeshRunnableOptions
{
    ~MultimeshTagOptions() override;
    utils::AttributeDescription tag_attribute;
    wmtk::attribute::MeshAttributeHandle::ValueVariant value;
    std::string output_mesh_name;
    bool delete_tag_attribute = false;
    FromTagOptions toTagOptions(const MeshCollection& mc) const;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshTagOptions)
    void run(MeshCollection& mc) const final;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json&) final;

    bool operator==(const MultimeshTagOptions&) const;
};

// returns the child mesh created
std::shared_ptr<Mesh> from_tag(
    const wmtk::attribute::MeshAttributeHandle& handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,
    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes);

std::shared_ptr<Mesh> from_tag(const FromTagOptions& options);
} // namespace wmtk::components::multimesh
