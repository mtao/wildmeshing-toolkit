#pragma once
#include "threshold.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct FilteredNeighborCountFunctor
{
    FilteredNeighborCountFunctor(const std::optional<InT>& over, const std::optional<InT>& under): m_threshold_functor(over,under)
    {
    }
    FilteredNeighborCountFunctor(const nlohmann::json& js)
        : m_threshold_functor(js)
    {}
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == InDim; }
    constexpr static bool validType()
    {
        return !std::is_same_v<InT, wmtk::Rational> && std::is_same_v<OutT, int64_t>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        using Vec = Vector<OutT, OutDim>;
        Vec v = Vec::Constant(M.rows(), 0);
        for(int j = 0; j < M.cols(); ++j) {
            auto m = M.col(j);
                v =v +  (m_threshold_functor(m).array() == true).select(Vec::Ones(M.rows()), Vec::Zero(M.rows())).matrix();
        }
        return v;
    }

private:
    ThresholdFunctor<InT,InDim,char,OutDim> m_threshold_functor;
};
template <>
inline int TransferFunctorTraits<FilteredNeighborCountFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return mah.dimension();
}
template <>
inline auto TransferFunctorTraits<FilteredNeighborCountFunctor>::output_type(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js) -> attribute::AttributeType
{
    return attribute::AttributeType::Char;
}
template <>
inline int TransferFunctorTraits<FilteredNeighborCountFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    assert(js.contains("dimension"));
    return js["dimension"];
}


using FilteredNeighborCount = SingleAttributeTransferStrategyFactory<FilteredNeighborCountFunctor>;

} // namespace wmtk::components::mesh_info::transfer
