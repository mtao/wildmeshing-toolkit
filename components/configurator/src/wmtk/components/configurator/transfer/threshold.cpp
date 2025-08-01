

#include "threshold.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::configurator::transfer {


template <>
SingleAttributeTransferStrategyFactory<ThresholdFunctor>::SingleAttributeTransferStrategyFactory() =
    default;
template <>
SingleAttributeTransferStrategyFactory<
    ThresholdFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::configurator::transfer
