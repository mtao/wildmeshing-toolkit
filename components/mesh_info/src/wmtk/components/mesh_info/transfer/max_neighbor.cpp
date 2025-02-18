#include "max_neighbor.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::mesh_info::transfer {


template <>
SingleAttributeTransferStrategyFactory<
    MaxNeighborFunctor>::SingleAttributeTransferStrategyFactory() = default;
template <>
SingleAttributeTransferStrategyFactory<
    MaxNeighborFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::mesh_info::transfer
