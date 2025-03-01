
#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"
#include "TransferFunctorTraits.hpp"

namespace wmtk::components::mesh_info::transfer {

// given k-1 simplex normals, produce a k-2 facet
template <typename InT, int InDim, typename OutT, int OutDim>
struct EdgeAngleFunctor
{
    EdgeAngleFunctor(const nlohmann::json& js) {}
    constexpr static bool validInDim()
    {
        return InDim == Eigen::Dynamic || InDim == 2 || InDim == 3;
    }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == 1; }
    constexpr static bool validType()
    {
        return std::is_same_v<double, InT> && std::is_same_v<double, OutT>;
    }
    // takes as input normals
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        if (M.cols() == 1) {
            return Vector<OutT, 1>::Constant(OutT(0));
        }
        assert(M.cols() == 2);

        auto p0 = M.col(0);
        auto p1 = M.col(1);
        if constexpr (InDim == 2) {
            auto cos = p1.dot(p0);

            auto sin = std::sqrt(p0.homogeneous().cross(p1.homogeneous()).z());

            return Vector<OutT, 1>::Constant(std::atan2(sin, cos));

        } else if constexpr (InDim == 3) {
            auto cos = p1.dot(p0);
            auto sin = p0.cross(p1).norm();
            return Vector<OutT, 1>::Constant(std::atan2(sin, cos));

        } else if constexpr (InDim == Eigen::Dynamic) {
            // run as static sizes
            if (M.rows() == 2) {
                return execute(M.template topRows<2>());
            } else if (M.rows() == 3) {
                return execute(M.template topRows<3>());
            }
        }
        assert(false);

        return {};
    }
    auto operator()(Eigen::Ref<const ColVectors<InT, InDim>> M) const -> Vector<OutT, OutDim>
    {
        return execute(M);
    }
};
template <>
struct TransferFunctorTraits<EdgeAngleFunctor>
{
    static int output_dimension(const attribute::MeshAttributeHandle& mah) { return 1; }
    static int simplex_dimension(const attribute::MeshAttributeHandle& mah , const nlohmann::json& js = {})
    {
        return mah.mesh().top_cell_dimension() - 2;
    }
};

using EdgeAngle = SingleAttributeTransferStrategyFactory<EdgeAngleFunctor>;

} // namespace wmtk::components::mesh_info::transfer
