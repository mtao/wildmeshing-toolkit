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
}

void SingleAttributeTransferStrategyFactoryBase::set_base_attribute(
    const multimesh::utils::AttributeDescription& at)
{
    SingleAttributeTransferStrategyParameters p;
    p.attribute = at;
    parameters = p;
}

auto SingleAttributeTransferStrategyFactoryBase::base_attribute() const
    -> multimesh::utils::AttributeDescription
{
    SingleAttributeTransferStrategyParameters p = parameters.get<SingleAttributeTransferStrategyParameters>();
    return p.attribute;
}

} // namespace wmtk::components::configurator::transfer
