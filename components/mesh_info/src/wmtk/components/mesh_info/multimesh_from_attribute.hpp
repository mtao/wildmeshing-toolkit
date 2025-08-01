#pragma once
#include <wmtk/components/configurator/transfer/TransferStrategyFactoryCollection.hpp>
#include <wmtk/components/multimesh/from_tag.hpp>


namespace wmtk::components::mesh_info {
struct MultimeshFromAttributeOptions
{
    wmtk::components::configurator::transfer::TransferStrategyFactoryCollection creation_attributes;
    multimesh::MultimeshTagOptions from_tag_options;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshFromAttributeOptions)

    void run(multimesh::MeshCollection& mc) const;
};


std::shared_ptr<Mesh> multimesh_from_attribute(multimesh::MeshCollection& mc, const MultimeshFromAttributeOptions& options);

} // namespace wmtk::components::mesh_info


