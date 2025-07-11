#include "TransferStrategyFactory.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategyBase.hpp>
#include "init.hpp"
namespace wmtk::components::mesh_info::transfer {

TransferStrategyFactoryRegistry TransferStrategyFactory::s_transfer_registry = create_registry();
TransferStrategyFactory::~TransferStrategyFactory() = default;
TransferStrategyFactory::TransferStrategyFactory() = default;
TransferStrategyFactory::TransferStrategyFactory(TransferStrategyFactory&& o) = default;
TransferStrategyFactory::TransferStrategyFactory(const TransferStrategyFactory& o)
    : attribute_path(o.attribute_path)
    , type(o.type)
//, dynamic_factory(o.dynamic_factory ? o.dynamic_factory->clone() : nullptr)
{}
TransferStrategyFactory& TransferStrategyFactory::operator=(TransferStrategyFactory&& o) = default;
TransferStrategyFactory& TransferStrategyFactory::operator=(const TransferStrategyFactory& o)
{
    this->attribute_path = o.attribute_path;
    this->type = o.type;
    // this->dynamic_factory = o.dynamic_factory->clone();
    return *this;
}
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> TransferStrategyFactory::create(
    wmtk::components::multimesh::MeshCollection& mc,
    bool populate) const
{
    auto t = create_transfer(mc);
    if (populate) {
        t->run_on_all();
    }
    return t;
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(TransferStrategyFactory)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute_path, type);
    nlohmann_json_t.to_json(nlohmann_json_j);
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(TransferStrategyFactory)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(attribute_path, type);
    nlohmann_json_t.from_json(nlohmann_json_j);
}
} // namespace wmtk::components::mesh_info::transfer
