#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "TransferFunctorTraits.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct MinNeighborFunctor
{
    MinNeighborFunctor(const nlohmann::json& js) {}
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim()
    {
        return OutDim == Eigen::Dynamic || InDim == Eigen::Dynamic || OutDim == InDim;
    }
    constexpr static bool validType() { return std::is_same_v<InT, OutT>; }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }


    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        return M.rowwise().minCoeff().template cast<OutT>();
    };
    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        return execute(M);
    }
};

template <>
struct TransferFunctorTraits<MinNeighborFunctor>
{
    static int output_dimension(const attribute::MeshAttributeHandle& mah)
    {
        return mah.dimension();
    }
    static int simplex_dimension(const attribute::MeshAttributeHandle& mah , const nlohmann::json& js = {})
    {
        return get_primitive_type_id(mah.primitive_type());
    }
};

using MinNeighbor = SingleAttributeTransferStrategyFactory<MinNeighborFunctor>;

} // namespace wmtk::components::mesh_info::transfer
