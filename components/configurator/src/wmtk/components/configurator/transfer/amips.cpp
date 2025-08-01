#include "amips.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::configurator::transfer {


template <>
SingleAttributeTransferStrategyFactory<
    AMIPSFunctor>::SingleAttributeTransferStrategyFactory() = default;
template <>
SingleAttributeTransferStrategyFactory<
    AMIPSFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::configurator::transfer
