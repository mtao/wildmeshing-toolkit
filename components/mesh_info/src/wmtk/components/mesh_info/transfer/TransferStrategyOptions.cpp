#include "TransferStrategyOptions.hpp"
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategyBase.hpp>
#include "TransferStrategy.hpp"
namespace wmtk::components::mesh_info::transfer {

TransferStrategyRegistry TransferStrategyOptions::s_transfer_registry;
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(TransferStrategyOptions){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute_path, type)

}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(TransferStrategyOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(attribute_path, type);

    nlohmann_json_t.parameters =
        TransferStrategyOptions::s_transfer_registry.create(nlohmann_json_t.type, nlohmann_json_j);
    assert(nlohmann_json_t.parameters);
}
TransferStrategyOptions::~TransferStrategyOptions() = default;
TransferStrategyOptions::TransferStrategyOptions() = default;
TransferStrategyOptions::TransferStrategyOptions(TransferStrategyOptions&& o) = default;
TransferStrategyOptions::TransferStrategyOptions(const TransferStrategyOptions& o)
    : attribute_path(o.attribute_path)
    , type(o.type)
    , parameters(o.parameters ? o.parameters->clone() : nullptr)
{}
TransferStrategyOptions& TransferStrategyOptions::operator=(TransferStrategyOptions&& o) = default;
TransferStrategyOptions& TransferStrategyOptions::operator=(const TransferStrategyOptions& o)
{
    this->attribute_path = o.attribute_path;
    this->type = o.type;
    this->parameters = o.parameters->clone();
    return *this;
}
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> TransferStrategyOptions::create(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    assert(parameters);
    auto t = parameters->create(mc, *this);
    spdlog::info("Creating attribute transfer {}", attribute_path);
    t->run_on_all();
    return t;
}
} // namespace wmtk::components::mesh_info::transfer
