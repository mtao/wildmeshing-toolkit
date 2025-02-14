#include "min_neighbor.hpp"
#include <nlohmann/json.hpp>
#include "TransferStrategyOptions.hpp"
#include "TransferStrategy_macros.hpp"
namespace wmtk::components::mesh_info::transfer {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(NeighborTransfer<MinNeighborFunctor>){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(base_attribute_path, simplex_dimension)}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(NeighborTransfer<MinNeighborFunctor>)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(base_attribute_path, simplex_dimension);
}

template <>
NeighborTransfer<MinNeighborFunctor>::NeighborTransfer() = default;
template <>
NeighborTransfer<MinNeighborFunctor>::~NeighborTransfer() = default;


WMTK_TRANSFER_ALL_DEFINITIONS_TEMPLATE(NeighborTransfer<MinNeighborFunctor>, "min_neighbor")

} // namespace wmtk::components::mesh_info::transfer
