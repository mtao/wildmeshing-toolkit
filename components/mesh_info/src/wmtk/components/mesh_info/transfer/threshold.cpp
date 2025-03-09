

#include "threshold.hpp"
#include <nlohmann/json.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
namespace wmtk::components::mesh_info::transfer {


template <>
SingleAttributeTransferStrategyFactory<ThresholdFunctor>::SingleAttributeTransferStrategyFactory() =
    default;
template <>
SingleAttributeTransferStrategyFactory<
    ThresholdFunctor>::~SingleAttributeTransferStrategyFactory() = default;


} // namespace wmtk::components::mesh_info::transfer
