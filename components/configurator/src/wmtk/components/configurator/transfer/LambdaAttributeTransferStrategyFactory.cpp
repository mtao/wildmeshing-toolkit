
#include "LambdaAttributeTransferStrategyFactory.hpp"
#include "wmtk/components/configurator/transfer/TransferStrategyOptions.hpp"


namespace wmtk::components::configurator::transfer {
LambdaFunctionTransferStrategyFactoryBase::LambdaFunctionTransferStrategyFactoryBase() = default;
LambdaFunctionTransferStrategyFactoryBase::~LambdaFunctionTransferStrategyFactoryBase() = default;

TransferStrategyOptions LambdaFunctionTransferStrategyFactoryBase::to_options() const
{
    return *this;
}
void LambdaFunctionTransferStrategyFactoryBase::from_options(const TransferStrategyOptions& opts)
{
    static_cast<TransferStrategyOptions&>(*this) = opts;
}

void LambdaFunctionTransferStrategyFactoryBase::set_base_attribute(
    const multimesh::utils::AttributeDescription& at)
{
    spdlog::info("Set parameters {}", at);
    SingleAttributeTransferStrategyParameters p;
    p.attribute = at;
    parameters = p;
    spdlog::info("Now paramters is {}", parameters.dump());
}

auto LambdaFunctionTransferStrategyFactoryBase::base_attribute() const
    -> multimesh::utils::AttributeDescription
{
    spdlog::info("{}", parameters.dump());
    SingleAttributeTransferStrategyParameters p = parameters;
    return p.attribute;
}

} // namespace wmtk::components::configurator::transfer
