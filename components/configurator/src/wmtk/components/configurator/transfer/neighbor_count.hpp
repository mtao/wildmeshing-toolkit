#pragma once
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "wmtk/attribute/MeshAttributeHandle.hpp"

namespace wmtk::components::configurator::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct NeighborCountFunctor
{

    NeighborCountFunctor(const nlohmann::json& js)
    {
    }
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == 1; }
    constexpr static bool validType()
    {
        return std::is_same_v<OutT, int64_t>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        return Vector<OutT, OutDim>::Constant(M.rows(), OutT(M.cols()));
    }

private:
    const std::optional<InT> m_over;
    const std::optional<InT> m_under;
};
template <>
inline int TransferFunctorTraits<NeighborCountFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return 1;
}
template <>
inline auto TransferFunctorTraits<NeighborCountFunctor>::output_type(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js) -> attribute::AttributeType
{
    return attribute::AttributeType::Int64;
}
template <>
inline int TransferFunctorTraits<NeighborCountFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    assert(js.contains("dimension"));
    return js["dimension"];
}


using NeighborCount = SingleAttributeTransferStrategyFactory<NeighborCountFunctor>;

} // namespace wmtk::components::configurator::transfer
