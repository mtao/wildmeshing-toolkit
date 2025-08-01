#pragma once
#include <Eigen/Dense>
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include <wmtk/function/utils/amips.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "TransferFunctorTraits.hpp"

namespace wmtk::components::configurator::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct AMIPSFunctor
{
    AMIPSFunctor(const nlohmann::json& js) {}
    constexpr static bool validInDim()
    {
        return InDim == Eigen::Dynamic || InDim == 2 || InDim == 3;
    }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == 1; }
    constexpr static bool validType()
    {
        return std::is_same_v<InT, double> && std::is_same_v<OutT, double>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }
    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        OutT v = 0;

        if constexpr (InDim == Eigen::Dynamic) {
            if (M.cols() == 2) {
                auto M_ = M.template topRows<2>();

                v = wmtk::function::utils::amips(M_.col(0), M_.col(1), M_.col(2));
            } else if (M.cols() == 3) {
                auto M_ = M.template topRows<3>();

                v = wmtk::function::utils::amips(M_.col(0), M_.col(1), M_.col(2));
            } else {
                assert(false);
            }
        } else {
            static_assert(InDim == 2 || InDim == 3);
            v = wmtk::function::utils::amips(M.col(0), M.col(1), M.col(2));
        }
        return Vector<OutT, 1>::Constant(v);
    };
    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        if constexpr (valid()) {
            return execute(M);
        }
    }
};

template <>
inline int TransferFunctorTraits<AMIPSFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return 1;
}
template <>
inline int TransferFunctorTraits<AMIPSFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    return mah.mesh().top_cell_dimension();
}
using AMIPS = SingleAttributeTransferStrategyFactory<AMIPSFunctor>;

} // namespace wmtk::components::configurator::transfer
