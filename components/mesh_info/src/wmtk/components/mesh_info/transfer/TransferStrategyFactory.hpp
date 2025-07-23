#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
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

    // runs the transfer operation / populates the data
    // calls create internally and tosses the resulting transfer, better to just call that if you
    // want to actually get the transfer
    wmtk::attribute::MeshAttributeHandle populate_attribute(
        wmtk::components::multimesh::MeshCollection&) const;
    wmtk::attribute::MeshAttributeHandle get_output_attribute(
        wmtk::components::multimesh::MeshCollection&) const;

    virtual components::multimesh::utils::AttributeDescription get_output_attribute_description(
        const wmtk::components::multimesh::MeshCollection&) const = 0;

    attribute::MeshAttributeHandle get_output_attribute_handle(
        const wmtk::components::multimesh::MeshCollection&) const;

    // virtual int output_dimension(int input_dim) const = 0;

    static const TransferStrategyFactoryRegistry& transfer_registry()
    {
        return s_transfer_registry;
    }

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
