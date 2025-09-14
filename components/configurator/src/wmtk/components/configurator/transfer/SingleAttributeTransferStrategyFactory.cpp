#include "SingleAttributeTransferStrategyFactory.hpp"


namespace wmtk::components::configurator::transfer {
SingleAttributeTransferStrategyFactoryBase::SingleAttributeTransferStrategyFactoryBase() = default;
SingleAttributeTransferStrategyFactoryBase::~SingleAttributeTransferStrategyFactoryBase() = default;

void SingleAttributeTransferStrategyFactoryBase::to_json(nlohmann::json& j) const
{
    j["attribute"] = attribute;
    j["type"] = type;
    j["base_attribute"] = base_attribute;
    j["parameters"] = parameters;
}
void SingleAttributeTransferStrategyFactoryBase::from_json(const nlohmann::json& j)
{
    spdlog::info("{}", j.dump());
    attribute = j["attribute"];
    type = j["type"];
    base_attribute = j["base_attribute"];
    if (j.contains("parameters")) {
        parameters = j["parameters"];
    }
}
} // namespace wmtk::components::configurator::transfer
