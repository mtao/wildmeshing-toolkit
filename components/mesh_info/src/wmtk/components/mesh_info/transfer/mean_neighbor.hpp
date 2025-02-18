#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct MeanNeighborFunctor
{
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim()
    {
        return OutDim == Eigen::Dynamic || InDim == Eigen::Dynamic || OutDim == InDim;
    }
    constexpr static bool validType() { return std::is_same_v<InT, OutT>; }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        return M.rowwise().mean().template cast<OutT>();
    };
};

using MeanNeighbor = SingleAttributeTransferStrategyFactory<MeanNeighborFunctor>;

} // namespace wmtk::components::mesh_info::transfer
