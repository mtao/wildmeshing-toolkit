#include "EdgeLengthTransferStrategyParameters.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/create_attribute.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>
#include "TransferStrategyOptions.hpp"
#include "TransferStrategyParameters_macros.hpp"
namespace wmtk::components::isotropic_remeshing::transfer {

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeLengthTransferStrategyParameters){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(position_attribute)}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeLengthTransferStrategyParameters)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(position_attribute);
}

EdgeLengthTransferStrategyParameters::EdgeLengthTransferStrategyParameters() = default;
EdgeLengthTransferStrategyParameters::~EdgeLengthTransferStrategyParameters() = default;


std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
EdgeLengthTransferStrategyParameters::create(
    wmtk::components::multimesh::MeshCollection& mc,

    const TransferStrategyOptions& opts) const
{
    auto pos_attr = wmtk::components::multimesh::utils::get_attribute(mc, {position_attribute});
    auto el_attr = wmtk::components::multimesh::utils::create_attribute(
        mc,
        {opts.attribute_path, 1, pos_attr.held_type(), 1});


    return std::visit(
        [&](const auto& attr_t) noexcept
            -> std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> {
            using T = typename std::decay_t<decltype(attr_t)>::Type;
            if constexpr (std::is_same_v<T, double>) {
                auto edge_length_func = [](const ColVectors<T, Eigen::Dynamic>& M) -> Vector<T, 1> {
                    return Vector<T, 1>::Constant((M.col(0) - M.col(1)).norm());
                };
                auto edge_length_func2 = [](const ColVectors<T, 2>& M) -> Vector<T, 1> {
                    return Vector<T, 1>::Constant((M.col(0) - M.col(1)).norm());
                };
                auto edge_length_func3 = [](const ColVectors<T, 3>& M) -> Vector<T, 1> {
                    return Vector<T, 1>::Constant((M.col(0) - M.col(1)).norm());
                };

                switch (pos_attr.dimension()) {
                case 2:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 1, 2>>

                        (el_attr, pos_attr, edge_length_func2);
                case 3:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 1, 3>>

                        (el_attr, pos_attr, edge_length_func3);
                default:
                    return std::make_shared<
                        wmtk::operations::SingleAttributeTransferStrategy<T, T, 1, Eigen::Dynamic>>

                        (el_attr, pos_attr, edge_length_func);

                    //
                }
            } else {
                assert(false);
                return {};
            }
        },
        pos_attr.handle());
}

WMTK_TRANSFER_ALL_DEFINITIONS(EdgeLengthTransferStrategyParameters, "edge_length")

} // namespace wmtk::components::isotropic_remeshing::transfer
