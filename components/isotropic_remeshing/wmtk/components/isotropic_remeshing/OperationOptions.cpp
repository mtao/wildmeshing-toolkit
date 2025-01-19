#include "OperationOptions.hpp"
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/create_attribute.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>

namespace wmtk::components::isotropic_remeshing {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PriorityOptions)
{
    //
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PriorityOptions)
{
    //
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(InvariantOptions){

    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type, mesh_path)}


WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(InvariantOptions)
{}


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(OperationOptions){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(mesh_path, enabled)

}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(OperationOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(OperationOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(mesh_path, enabled);

    //
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeSplitOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSplitOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeCollapseOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeCollapseOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeSwapOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
    std::string name;
    switch (nlohmann_json_t.mode) {
    case EdgeSwapMode::AMIPS: name = "amips"; break;
    case EdgeSwapMode::Valence: name = "valence"; break;
    default:
    case EdgeSwapMode::Skip: name = "skip"; break;
    }
    nlohmann_json_j["mode"] = name;
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeSwapOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
    assert(nlohmann_json_j.contains("mode"));
    const std::string swap_name = nlohmann_json_j["mode"].get<std::string>();
    if (swap_name == "amips") {
        nlohmann_json_t.mode = EdgeSwapMode::AMIPS;
    } else if (swap_name == "valence") {
        nlohmann_json_t.mode = EdgeSwapMode::Valence;
    } else if (swap_name == "skip") {
        nlohmann_json_t.mode = EdgeSwapMode::Skip;
    } else {
        throw std::runtime_error(fmt::format(
            "Expected edge_swap_mode to be one of [amips,valence,skip], got [{}]",
            swap_name));
    }
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(VertexSmoothOptions)
{
    to_json(nlohmann_json_j, static_cast<const OperationOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(VertexSmoothOptions)
{
    from_json(nlohmann_json_j, static_cast<OperationOptions&>(nlohmann_json_t));
}


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(TransferStrategyOptions){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute_path, type)

}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(TransferStrategyOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(TransferStrategyOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(attribute_path, type);

    if (nlohmann_json_t.type == ("edge_length")) {
        assert(nlohmann_json_j.contains("parameters"));
        nlohmann_json_t.data = std::make_unique<EdgeLengthTransferStrategyData>();
        nlohmann_json_t.data->from_json(nlohmann_json_j["parameters"]);
    }
}

TransferStrategyData::~TransferStrategyData() = default;

#define TRANSFER_STRATEGY_CHILD_FORWARD(NAME)                        \
    void NAME::to_json(nlohmann::json& js) const                     \
    {                                                                \
        wmtk::components::isotropic_remeshing::to_json(js, *this);   \
    }                                                                \
    void NAME::from_json(const nlohmann::json& js)                   \
    {                                                                \
        wmtk::components::isotropic_remeshing::from_json(js, *this); \
    }


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EdgeLengthTransferStrategyData){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(position_attribute)}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EdgeLengthTransferStrategyData)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(EdgeLengthTransferStrategyData);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(position_attribute);
}
TRANSFER_STRATEGY_CHILD_FORWARD(EdgeLengthTransferStrategyData)
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> TransferStrategyOptions::create(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    assert(data);
    return data->create(mc, *this);
}

EdgeLengthTransferStrategyData::EdgeLengthTransferStrategyData() = default;
EdgeLengthTransferStrategyData::~EdgeLengthTransferStrategyData() = default;

std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
EdgeLengthTransferStrategyData::create(
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

std::unique_ptr<TransferStrategyData> EdgeLengthTransferStrategyData::clone() const
{
    return std::make_unique<EdgeLengthTransferStrategyData>(*this);
}

TransferStrategyOptions::TransferStrategyOptions(const TransferStrategyOptions& o)
    : attribute_path(o.attribute_path)
    , type(o.type)
    , data(o.data->clone())
{}
TransferStrategyOptions& TransferStrategyOptions::operator=(const TransferStrategyOptions& o)
{
    this->attribute_path = o.attribute_path;
    this->type = o.type;
    this->data = o.data->clone();
    return *this;
}
} // namespace wmtk::components::isotropic_remeshing
