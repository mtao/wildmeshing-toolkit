#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct MeanRatioMeasureFunctor
{
    MeanRatioMeasureFunctor(const nlohmann::json& js) {}
    constexpr static bool validInDim() { return true; }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == 1; }
    constexpr static bool validType()
    {
        return std::is_same_v<double, InT> && std::is_same_v<double, OutT>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        auto p0 = M.col(0);
        auto p1 = M.col(1);
        auto p2 = M.col(2);

        const auto a = (p1 - p0).eval();
        const auto b = (p2 - p1).eval();
        const auto c = (p0 - p2).eval();

        const double sq_length_sum = a.squaredNorm() + b.squaredNorm() + c.squaredNorm();

        double area;
        if constexpr (InDim == 2) {
            area = std::sqrt(a.homogeneous().cross(b.homogeneous()).z());
        } else if constexpr (InDim == 3) {
            area = a.cross(b).norm();
        } else if constexpr (InDim == Eigen::Dynamic) {
            if (M.rows() == 2) {
                area = std::sqrt(a.template head<2>()
                                     .homogeneous()
                                     .cross(b.template head<2>().homogeneous())
                                     .z());
            } else if (M.rows() == 3) {
                area = a.template head<3>().cross(b.template head<3>()).norm();
            } else {
                area = 0;
                assert(false);
            }
        } else {
            assert(false);
        }

        const double prefactor = 2 * std::sqrt(3.0);
        // const double prefactor = 2 * std::numbers::sqrt3;

        return Eigen::Vector<OutT, 1>::Constant(prefactor * area / sq_length_sum);
    }
    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        return execute(M);
    }
};

template <>
struct TransferFunctorTraits<MeanRatioMeasureFunctor>
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

using MeanRatioMeasure = SingleAttributeTransferStrategyFactory<MeanRatioMeasureFunctor>;

} // namespace wmtk::components::mesh_info::transfer
