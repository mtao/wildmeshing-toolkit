#pragma once
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "wmtk/attribute/MeshAttributeHandle.hpp"

namespace wmtk::components::configurator::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct NegateFunctor
{
    NegateFunctor(const nlohmann::json& js)
    {}
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == InDim; }
    constexpr static bool validType()
    {
        return !std::is_same_v<InT, wmtk::Rational> && std::is_same_v<OutT, char>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        assert(M.cols() == 1);
        return !M.array();
    }

private:
    const std::optional<InT> m_over;
    const std::optional<InT> m_under;
};
template <>
inline int TransferFunctorTraits<NegateFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return mah.dimension();
}
template <>
inline auto TransferFunctorTraits<NegateFunctor>::output_type(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js) -> attribute::AttributeType
{
    return attribute::AttributeType::Char;
}
template <>
inline int TransferFunctorTraits<NegateFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    return get_primitive_type_id(mah.primitive_type());
}


using Negate = SingleAttributeTransferStrategyFactory<NegateFunctor>;

} // namespace wmtk::components::configurator::transfer
