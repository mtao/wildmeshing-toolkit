#include "SingleAttributeTransferStrategyFactory.hpp"
#include "wmtk/components/configurator/transfer/TransferStrategyOptions.hpp"


namespace wmtk::components::configurator::transfer {
SingleAttributeTransferStrategyFactoryBase::SingleAttributeTransferStrategyFactoryBase() = default;
SingleAttributeTransferStrategyFactoryBase::~SingleAttributeTransferStrategyFactoryBase() = default;

TransferStrategyOptions SingleAttributeTransferStrategyFactoryBase::to_options() const
{
    return *this;
}
void SingleAttributeTransferStrategyFactoryBase::from_options(const TransferStrategyOptions& opts)
{
    auto& me = static_cast<TransferStrategyOptions&>(*this);
     me = opts;
    spdlog::info("from options got stored as {}",nlohmann::json(me).dump(2));
}

void SingleAttributeTransferStrategyFactoryBase::set_base_attribute(
    const multimesh::utils::AttributeDescription& at)
{
    spdlog::info("Set parameters {}", at);
    SingleAttributeTransferStrategyParameters p;
    p.attribute = at;
    parameters = p;
    spdlog::info("Now paramters is {}", parameters.dump());
}

auto SingleAttributeTransferStrategyFactoryBase::base_attribute() const
    -> multimesh::utils::AttributeDescription
{
    spdlog::info("SingleAttributeTransferStrategyParameters::base_attribute() params are {}", parameters.dump());
    SingleAttributeTransferStrategyParameters p = parameters;
    return p.attribute;
}

} // namespace wmtk::components::configurator::transfer
