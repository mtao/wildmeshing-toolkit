#include "multimesh_from_attribute.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/multimesh/utils/extract_child_mesh_from_tag.hpp>
#include <wmtk/multimesh/utils/transfer_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>


namespace wmtk::components::mesh_info {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MultimeshFromAttributeOptions)
{

    nlohmann_json_j["creation_attributes"] = nlohmann_json_t.creation_attributes;
    nlohmann_json_j["from_tag_options"] = nlohmann_json_t.from_tag_options;
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MultimeshFromAttributeOptions)
{

    nlohmann_json_t.creation_attributes = nlohmann_json_j["creation_attributes"];
    nlohmann_json_t.from_tag_options = nlohmann_json_j["from_tag_options"].get<multimesh::MultimeshTagOptions>();
}
void MultimeshFromAttributeOptions::run(multimesh::MeshCollection& mc) const
{
    std::vector<wmtk::attribute::MeshAttributeHandle> attributes_to_delete;
     for (const auto& transfer : creation_attributes) {
         attributes_to_delete.emplace_back(transfer->create(mc)->handle());
     }

    from_tag_options.run(mc);

    for (auto& attr : attributes_to_delete) {
        attr.mesh().delete_attribute(attr);
    }
}

std::shared_ptr<Mesh> multimesh_from_attribute(multimesh::MeshCollection& mc, const MultimeshFromAttributeOptions& options) {

    options.run(mc);
    return nullptr;
}
} // namespace wmtk::components::mesh_info
