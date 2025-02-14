#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "NeighborTransfer.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename T, int InDim, int OutDim>
struct MinNeighborFunctor
{
    static_assert(InDim == OutDim);
    static auto operator()(const ColVectors<T, InDim>& M) -> Vector<T, OutDim>
    {
        return M.rowwise().minCoeff();
    };
};

using MinNeighbor = NeighborTransfer<MinNeighborFunctor>;

} // namespace wmtk::components::mesh_info::transfer
