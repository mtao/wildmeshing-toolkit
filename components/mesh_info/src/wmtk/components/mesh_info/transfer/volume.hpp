#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "TransferFunctorTraits.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct VolumeFunctor
{
    VolumeFunctor(const nlohmann::json& js) {}
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == 1; }
    constexpr static bool validType()
    {
        return std::is_same_v<InT, double> && std::is_same_v<OutT, double>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }
    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        constexpr static int factorial[] = {1, 1, 2, 6, 24};
        OutT value = 0;
        if (M.rows() + 1 == M.cols()) {
            auto m = (M.rightCols(M.cols() - 1).colwise() - M.col(0)).eval();
            value = m.determinant() / factorial[m.cols()];
        } else {
            auto m = (M.rightCols(M.cols() - 1).colwise() - M.col(0)).eval();
            int quotient = factorial[m.cols()];
            value = std::sqrt((m.transpose() * m).determinant()) / quotient;
        }
        return Vector<OutT, 1>::Constant(value);
    };
    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        if constexpr (valid()) {
            return execute(M);
        }
    }
};

template <>
struct TransferFunctorTraits<VolumeFunctor>
{
    constexpr static int output_dimension(int D) { return 1; }
};
using Volume = SingleAttributeTransferStrategyFactory<VolumeFunctor>;

} // namespace wmtk::components::mesh_info::transfer
