#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategyFactoryRegistry.hpp"
namespace wmtk {
namespace operations {
class AttributeTransferStrategyBase;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::mesh_info::transfer {

struct TransferStrategyFactory
{
    TransferStrategyFactory();
    virtual ~TransferStrategyFactory();
    TransferStrategyFactory(const TransferStrategyFactory&);
    TransferStrategyFactory(TransferStrategyFactory&&);
    TransferStrategyFactory& operator=(const TransferStrategyFactory&);
    TransferStrategyFactory& operator=(TransferStrategyFactory&&);
    std::string attribute_path;
    std::string type;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(TransferStrategyFactory)


    virtual std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc,
        bool populate = true) const;

protected:
    virtual std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_transfer(
        wmtk::components::multimesh::MeshCollection& mc) const = 0;

    friend struct TransferStrategy;
    friend void init();

    virtual void to_json(nlohmann::json&) const = 0;
    virtual void from_json(const nlohmann::json&) = 0;

    template <typename Type>
    void static register_transfer(const std::string& name);

private:
    static TransferStrategyFactoryRegistry s_transfer_registry;
};
void to_json(nlohmann::json&, const TransferStrategyFactory&);
void from_json(const nlohmann::json&, TransferStrategyFactory&);

template <typename Type>
void TransferStrategyFactory::register_transfer(const std::string& name)
{
    s_transfer_registry.register_transfer<Type>(name);
}
} // namespace wmtk::components::mesh_info::transfer
