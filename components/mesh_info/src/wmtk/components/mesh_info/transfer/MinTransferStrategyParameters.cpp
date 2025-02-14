#include "MinTransferStrategyParameters.hpp"
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

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MinTransferStrategyParameters){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(base_attribute_path, simplex_dimension)}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MinTransferStrategyParameters)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(base_attribute_path, simplex_dimension);
}

MinTransferStrategyParameters::MinTransferStrategyParameters() = default;
MinTransferStrategyParameters::~MinTransferStrategyParameters() = default;

namespace {
template <typename T, int Dim>
auto mean_neighbor(const ColVectors<T, Dim>& M) -> Vector<T, Dim>
{
    return M.rowwise().minCoeff();
};
} // namespace

std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
MinTransferStrategyParameters::create(
    wmtk::components::multimesh::MeshCollection& mc,

    const TransferStrategyOptions& opts) const
{
    auto pos_attr = wmtk::components::multimesh::utils::get_attribute(mc, {base_attribute_path});
    auto el_attr = wmtk::components::multimesh::utils::create_attribute(
        mc,
        {opts.attribute_path, simplex_dimension, pos_attr.held_type(), pos_attr.dimension()});


    return std::visit(
        [&](const auto& attr_t) noexcept
            -> std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> {
            using T = typename std::decay_t<decltype(attr_t)>::Type;
            if constexpr (std::is_same_v<T, double>) {

                switch (pos_attr.dimension()) {
                case 2:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 2, 2>>

                        (el_attr, pos_attr, &mean_neighbor<T, 2>);
                case 3:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 3, 3>>

                        (el_attr, pos_attr, &mean_neighbor<T, 3>);
                default:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, Eigen::Dynamic, Eigen::Dynamic>>

                        (el_attr, pos_attr, &mean_neighbor<T, Eigen::Dynamic>);

                    //
                }
            } else {
                assert(false);
                return {};
            }
        },
        pos_attr.handle());
}

WMTK_TRANSFER_ALL_DEFINITIONS(MinTransferStrategyParameters, "mean_neighbor")

} // namespace wmtk::components::mesh_info::transfer
