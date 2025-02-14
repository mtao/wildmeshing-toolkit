#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategyRegistry.hpp"
namespace wmtk {
namespace operations {
class AttributeTransferStrategyBase;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::mesh_info::transfer {
struct TransferStrategy;
struct TransferStrategyOptions
{
    TransferStrategyOptions();
    ~TransferStrategyOptions();
    TransferStrategyOptions(const TransferStrategyOptions&);
    TransferStrategyOptions(TransferStrategyOptions&&);
    TransferStrategyOptions& operator=(const TransferStrategyOptions&);
    TransferStrategyOptions& operator=(TransferStrategyOptions&&);
    std::string attribute_path;
    std::string type;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(TransferStrategyOptions)
    std::unique_ptr<TransferStrategy> parameters;
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc) const;

    friend struct TransferStrategy;
    friend void init();

    template <typename Type>
    void static register_transfer(const std::string& name);

private:
    static TransferStrategyRegistry s_transfer_registry;
};

template <typename Type>
void TransferStrategyOptions::register_transfer(const std::string& name)
{
    s_transfer_registry.register_transfer<Type>(name);
}
} // namespace wmtk::components::mesh_info::transfer
