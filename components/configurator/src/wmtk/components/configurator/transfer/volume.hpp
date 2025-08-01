#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include <Eigen/Dense>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "TransferFunctorTraits.hpp"

namespace wmtk::components::configurator::transfer {

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
inline int TransferFunctorTraits<VolumeFunctor>::output_dimension(
    const attribute::MeshAttributeHandle& mah)
{
    return 1;
}
template <>
inline int TransferFunctorTraits<VolumeFunctor>::simplex_dimension(
    const attribute::MeshAttributeHandle& mah,
    const nlohmann::json& js)
{
    int dim;
    if (js.contains("simplex_dimension")) {
        dim = js["simplex_dimension"];
    } else {
        dim = get_primitive_type_id(mah.mesh().top_simplex_type());
    }
    return dim;
}
using Volume = SingleAttributeTransferStrategyFactory<VolumeFunctor>;

} // namespace wmtk::components::configurator::transfer
