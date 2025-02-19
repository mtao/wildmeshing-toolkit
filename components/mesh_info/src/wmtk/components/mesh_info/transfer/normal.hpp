#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "SingleAttributeTransferStrategyFactory.hpp"

namespace wmtk::components::mesh_info::transfer {

template <typename InT, int InDim, typename OutT, int OutDim>
struct NormalFunctor
{
    NormalFunctor(const nlohmann::json&) {}
    constexpr static bool validInDim()
    {
        return InDim == Eigen::Dynamic || InDim == 2 || InDim == 3;
    }
    constexpr static bool validOutDim() { return OutDim == Eigen::Dynamic || OutDim == InDim; }
    constexpr static bool validType()
    {
        return std::is_same_v<double, InT> && std::is_same_v<double, OutT>;
    }
    constexpr static bool valid() { return validInDim() && validOutDim() && validType(); }

    static auto execute(Eigen::Ref<const ColVectors<InT, InDim>> M) -> Vector<OutT, OutDim>
    {
        assert(M.cols() == M.rows());

        if constexpr (InDim == 2) {
            auto p0 = M.col(0);
            auto p1 = M.col(1);
            const auto a = (p1 - p0).eval();

            return Vector<OutT, 2>(-a.y(), a.x());
        } else if constexpr (InDim == 3) {
            auto p0 = M.col(0);
            auto p1 = M.col(1);
            auto p2 = M.col(2);

            const auto a = (p1 - p0).eval();
            const auto b = (p2 - p0).eval();
            return a.cross(b).normalized();
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
    struct TransferFunctorTraits<NormalFunctor> {
        constexpr static int output_dimension(int D) {
            return D;
        }
    };

using Normal = SingleAttributeTransferStrategyFactory<NormalFunctor>;

} // namespace wmtk::components::mesh_info::transfer
