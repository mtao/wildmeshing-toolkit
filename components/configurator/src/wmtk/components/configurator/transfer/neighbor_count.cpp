
#include "neighbor_count.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::configurator::transfer {


template <>
SingleAttributeTransferStrategyFactory<NeighborCountFunctor>::SingleAttributeTransferStrategyFactory() =
    default;
template <>
SingleAttributeTransferStrategyFactory<
    NeighborCountFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::configurator::transfer
