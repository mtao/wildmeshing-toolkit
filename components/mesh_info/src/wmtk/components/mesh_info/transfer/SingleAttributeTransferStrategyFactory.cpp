#include "SingleAttributeTransferStrategyFactory.hpp"


namespace wmtk::components::mesh_info::transfer {
SingleAttributeTransferStrategyFactoryBase::SingleAttributeTransferStrategyFactoryBase() = default;
SingleAttributeTransferStrategyFactoryBase::~SingleAttributeTransferStrategyFactoryBase() = default;

void SingleAttributeTransferStrategyFactoryBase::to_json(nlohmann::json& j) const
{
    j["attribute_path"] = attribute_path;
    j["type"] = type;
    j["base_attribute_path"] = base_attribute_path;
    j["parameters"] = parameters;
}
void SingleAttributeTransferStrategyFactoryBase::from_json(const nlohmann::json& j)
{
    spdlog::warn("{}", j.dump(2));
    attribute_path = j["attribute_path"];
    type = j["type"];
    base_attribute_path = j["base_attribute_path"].get<std::string>();
    if(j.contains("parameters")) {
    parameters = j["parameters"];
    }
}
attribute::AttributeType SingleAttributeTransferStrategyFactoryBase::output_type(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    auto pos_attr = wmtk::components::multimesh::utils::get_attribute(mc, {base_attribute_path});
    return pos_attr.held_type();
}
// int SingleAttributeTransferStrategyFactoryBase::base_attribute_dimension(
//     wmtk::components::multimesh::MeshCollection& mc) const
//{
//     auto pos_attr = wmtk::components::multimesh::utils::get_attribute(mc, {base_attribute_path});
//     return pos_attr.dimension();
// }
// int SingleAttributeTransferStrategyFactoryBase::base_simplex_dimension(
//     wmtk::components::multimesh::MeshCollection& mc) const
//{
//     auto pos_attr = wmtk::components::multimesh::utils::get_attribute(mc, {base_attribute_path});
//     return get_primitive_type_id(pos_attr.primitive_type());
// }
} // namespace wmtk::components::mesh_info::transfer
