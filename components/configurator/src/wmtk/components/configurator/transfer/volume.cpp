#include "volume.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::configurator::transfer {


template <>
SingleAttributeTransferStrategyFactory<
    VolumeFunctor>::SingleAttributeTransferStrategyFactory() = default;
template <>
SingleAttributeTransferStrategyFactory<
    VolumeFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::configurator::transfer
