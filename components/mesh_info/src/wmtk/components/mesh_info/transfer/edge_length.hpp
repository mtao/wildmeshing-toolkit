#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "TransferFunctorTraits.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct EdgeLengthFunctor
{
    EdgeLengthFunctor(const nlohmann::json& js) {}
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == 1; }
    constexpr static bool validType()
    {
        return !std::is_same_v<InT, wmtk::Rational> && !std::is_same_v<OutT, wmtk::Rational>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }
    static auto execute(Eigen::Ref<const Eigen::Matrix<InT, InDim, 2>> M) -> Vector<OutT, OutDim>
    {
        return Vector<OutT, 1>::Constant((M.col(0) - M.col(1)).norm());
    };
    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        return execute(M);
    }
};

template <>
inline int TransferFunctorTraits<EdgeLengthFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return 1;
}
template <>
inline int TransferFunctorTraits<EdgeLengthFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    return 1;
}
using EdgeLength = SingleAttributeTransferStrategyFactory<EdgeLengthFunctor>;

} // namespace wmtk::components::mesh_info::transfer
