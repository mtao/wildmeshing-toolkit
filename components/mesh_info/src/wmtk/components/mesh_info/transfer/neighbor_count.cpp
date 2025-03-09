
#include "neighbor_count.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::mesh_info::transfer {


template <>
SingleAttributeTransferStrategyFactory<NeighborCountFunctor>::SingleAttributeTransferStrategyFactory() =
    default;
template <>
SingleAttributeTransferStrategyFactory<
    NeighborCountFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::mesh_info::transfer
