#include "edge_length.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::mesh_info::transfer {


template <>
SingleAttributeTransferStrategyFactory<
    EdgeLengthFunctor>::SingleAttributeTransferStrategyFactory() = default;
template <>
SingleAttributeTransferStrategyFactory<
    EdgeLengthFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::mesh_info::transfer
