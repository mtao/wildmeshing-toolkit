#include "SingleAttributeTransferStrategyFactory.hpp"


namespace wmtk::components::configurator::transfer {
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
    attribute_path = j["attribute_path"];
    type = j["type"];
    base_attribute_path = j["base_attribute_path"].get<std::string>();
    if (j.contains("parameters")) {
        parameters = j["parameters"];
    }
}
} // namespace wmtk::components::configurator::transfer
