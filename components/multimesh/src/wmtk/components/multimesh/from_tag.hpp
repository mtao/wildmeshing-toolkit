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
namespace components::mesh_info::transfer {

class TransferStrategyFactoryCollection;
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
    bool manifold_decomposition = false;
};

// json serializable version
struct MultimeshTagOptions : public MultimeshRunnableOptions
{
    MultimeshTagOptions();
    MultimeshTagOptions(MultimeshTagOptions&&);
    MultimeshTagOptions& operator=(MultimeshTagOptions&&);
    ~MultimeshTagOptions() override;
    utils::AttributeDescription tag_attribute;
    wmtk::attribute::MeshAttributeHandle::ValueVariant value;
    std::vector<utils::AttributeDescription> passed_attributes;
    std::string output_mesh_name;
    bool delete_tag_attribute = true;
    bool manifold_decomposition = true;
    FromTagOptions toTagOptions(const MeshCollection& mc) const;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshTagOptions)
    void run(MeshCollection& mc) const final;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json&) final;
    std::unique_ptr<mesh_info::transfer::TransferStrategyFactoryCollection> creation_attributes;

    bool operator==(const MultimeshTagOptions&) const;
};

// returns the child mesh created
std::shared_ptr<Mesh> from_tag(
    const wmtk::attribute::MeshAttributeHandle& handle,
    const wmtk::attribute::MeshAttributeHandle::ValueVariant& tag_value,
    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes = {},
    bool manifold_decomposition = false);

std::shared_ptr<Mesh> from_tag(const FromTagOptions& options);
} // namespace wmtk::components::multimesh
