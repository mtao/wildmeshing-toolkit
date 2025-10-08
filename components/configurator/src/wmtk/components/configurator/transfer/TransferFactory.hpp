#pragma once
#include "TransferStrategyFactoryRegistry.hpp"

#include <wmtk/operations/attribute_update/AttributeTransferStrategyBase.hpp>

namespace wmtk::components::configurator {
class Configurator;
}

namespace wmtk::components::configurator::transfer {


struct TransferFactory
{
    using transfer_strategy_type = wmtk::operations::AttributeTransferStrategyBase;
    TransferFactory();


    // void add(const std::string& s, const void& f, std::string_view info = {});


    void from_json(Configurator&, const nlohmann::json& js);


    std::shared_ptr<transfer_strategy_type>
    create(Configurator& config, std::string_view name, const nlohmann::json& js);

    std::shared_ptr<transfer_strategy_type>
    create(Configurator& config, std::string_view name, const TransferStrategyOptions& opt);

    std::shared_ptr<transfer_strategy_type> create(Configurator& config, const nlohmann::json& js);

    std::shared_ptr<transfer_strategy_type> get(const std::string& name);

    std::string_view get_name(const transfer_strategy_type& op) const;

    std::vector<std::string> known_transfer_functors() const;
    std::vector<std::string> known_transfers() const;


    auto& registry() { return *m_registry; }
    auto& registry() const { return *m_registry; }


private:
    // TODO: this was written before the OperationFactory and InvariantFactory so ownership of the
    // functions + holding of the operations
    std::shared_ptr<transfer::TransferStrategyFactoryRegistry> m_registry;
    std::map<
        std::string,
        std::pair<std::shared_ptr<transfer_strategy_type>, TransferStrategyOptions>>
        m_operation_transfers;
};

} // namespace wmtk::components::configurator::transfer
