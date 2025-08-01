
#include "normal.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::configurator::transfer {


template <>
SingleAttributeTransferStrategyFactory<NormalFunctor>::SingleAttributeTransferStrategyFactory() =
    default;
template <>
SingleAttributeTransferStrategyFactory<NormalFunctor>::~SingleAttributeTransferStrategyFactory() =
    default;


} // namespace wmtk::components::configurator::transfer
