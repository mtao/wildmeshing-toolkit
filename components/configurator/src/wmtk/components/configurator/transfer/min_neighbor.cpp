#include "min_neighbor.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::configurator::transfer {


template <>
SingleAttributeTransferStrategyFactory<
    MinNeighborFunctor>::SingleAttributeTransferStrategyFactory() = default;
template <>
SingleAttributeTransferStrategyFactory<
    MinNeighborFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::configurator::transfer
