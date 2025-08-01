#pragma once
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "wmtk/attribute/MeshAttributeHandle.hpp"

namespace wmtk::components::configurator::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct ThresholdFunctor
{
    ThresholdFunctor(const std::optional<InT>& over, const std::optional<InT>& under)
        : m_over(over)
        , m_under(under)
    {
        assert(m_under.has_value() || m_over.has_value());
    }
    ThresholdFunctor(const nlohmann::json& js)
        : ThresholdFunctor(
              js.contains("over") ? js["over"].get<InT>() : std::optional<InT>{},
              js.contains("under") ? js["under"].get<InT>() : std::optional<InT>{})
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
        const auto& A = M.array();
        if (m_over.has_value() && m_under.has_value()) {
            const auto& lower = m_over.value();
            const auto& upper = m_under.value();
            return (lower <= A && A <= upper).template cast<char>();
        } else if (m_over.has_value()) {
            const auto& lower = m_over.value();
            return (lower <= A).template cast<char>();
        } else {
            const auto& upper = m_under.value();
            return (A <= upper).template cast<char>();
        }
        return Vector<OutT, OutDim>::Constant(M.rows(), 0);
    }

private:
    const std::optional<InT> m_over;
    const std::optional<InT> m_under;
};
template <>
inline int TransferFunctorTraits<ThresholdFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return mah.dimension();
}
template <>
inline auto TransferFunctorTraits<ThresholdFunctor>::output_type(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js) -> attribute::AttributeType
{
    return attribute::AttributeType::Char;
}
template <>
inline int TransferFunctorTraits<ThresholdFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    return get_primitive_type_id(mah.primitive_type());
}


using Threshold = SingleAttributeTransferStrategyFactory<ThresholdFunctor>;

} // namespace wmtk::components::configurator::transfer
