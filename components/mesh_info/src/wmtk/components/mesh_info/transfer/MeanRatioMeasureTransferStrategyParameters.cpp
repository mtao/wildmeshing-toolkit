#include "MeanRatioMeasureTransferStrategyParameters.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/create_attribute.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>
#include "TransferStrategyOptions.hpp"
#include "TransferStrategyParameters_macros.hpp"
namespace wmtk::components::mesh_info::transfer {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MeanRatioMeasureTransferStrategy){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(position_attribute)}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MeanRatioMeasureTransferStrategy)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(position_attribute);
}

MeanRatioMeasureTransferStrategy::MeanRatioMeasureTransferStrategy() = default;
MeanRatioMeasureTransferStrategy::~MeanRatioMeasureTransferStrategy() = default;

namespace {
template <typename T, int Dim>
auto mean_area_measure(const ColVectors<T, Dim>& M) -> Vector<T, 1>
{
    auto p0 = M.col(0);
    auto p1 = M.col(1);
    auto p2 = M.col(2);

    const auto a = (p1 - p0).eval();
    const auto b = (p2 - p1).eval();
    const auto c = (p0 - p2).eval();

    const double sq_length_sum = a.squaredNorm() + b.squaredNorm() + c.squaredNorm();

    double area;
    if constexpr (Dim == 2) {
        area = std::sqrt(a.homogeneous().cross(b.homogeneous()).z());
    } else if constexpr (Dim == 3) {
        area = a.cross(b).norm();
    } else if constexpr (Dim == Eigen::Dynamic) {
        if (M.rows() == 2) {
            area = std::sqrt(
                a.template head<2>().homogeneous().cross(b.template head<2>().homogeneous()).z());
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
    //const double prefactor = 2 * std::numbers::sqrt3;

    return Eigen::Vector<T, 1>::Constant(prefactor * area / sq_length_sum);
};
} // namespace

std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
MeanRatioMeasureTransferStrategy::create(
    wmtk::components::multimesh::MeshCollection& mc,

    const TransferStrategyOptions& opts) const
{
    auto pos_attr = wmtk::components::multimesh::utils::get_attribute(mc, {position_attribute});
    wmtk::components::multimesh::utils::AttributeDescription ad{opts.attribute_path, 2, pos_attr.held_type(), 1};

    auto el_attr = wmtk::components::multimesh::utils::create_attribute(
        mc,
        ad);


    return std::visit(
        [&](const auto& attr_t) noexcept
            -> std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> {
            using T = typename std::decay_t<decltype(attr_t)>::Type;
            if constexpr (std::is_same_v<T, double>) {

                switch (pos_attr.dimension()) {
                case 2:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 1, 2>>

                        (el_attr, pos_attr, &mean_area_measure<T, 2>);
                case 3:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 1, 3>>

                        (el_attr, pos_attr, &mean_area_measure<T, 3>);
                default:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 1, Eigen::Dynamic>>

                        (el_attr, pos_attr, &mean_area_measure<T, Eigen::Dynamic>);

                    //
                }
            } else {
                assert(false);
                return {};
            }
        },
        pos_attr.handle());
}

WMTK_TRANSFER_ALL_DEFINITIONS(MeanRatioMeasureTransferStrategy, "mean_ratio_measure")

} // namespace wmtk::components::mesh_info::transfer
