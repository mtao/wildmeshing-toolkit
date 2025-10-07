#include "TransferStrategyFactoryCollection.hpp"
#include "TransferStrategyFactoryRegistry.hpp"
#include <nlohmann/json.hpp>
namespace wmtk::components::configurator::transfer {

void to_json(nlohmann::json& j, const TransferStrategyFactoryCollection& value)
{
    // calls the "to_json" method in T's namespace
    std::vector<nlohmann::json> arr;
    std::transform(
        value.begin(),
        value.end(),
        std::back_inserter(arr),
        [](auto&& a) -> nlohmann::json { return *a; });
    j = arr;
}

void from_json(const nlohmann::json& j, TransferStrategyFactoryCollection& value)
{
    if (!j.is_array()) {
        throw std::runtime_error("utility attributes must be an array");
    }
    const auto& registry =
        wmtk::components::configurator::transfer::TransferStrategyFactory::transfer_registry();
    for (const auto& attr : j) {
        value.emplace_back(registry.create(attr));
    }
}

void to_json(nlohmann::json& j, const NamedTransferStrategyFactoryCollection& value)
{
    // calls the "to_json" method in T's namespace
    std::map<std::string, nlohmann::json> arr;
    std::transform(
        value.begin(),
        value.end(),
        std::inserter(arr, arr.end()),
        [](auto&& a) -> std::pair<const std::string, nlohmann::json> {
            return {a.first, nlohmann::json(*a.second)};
        });
    j = arr;
}

void from_json(const nlohmann::json& j, NamedTransferStrategyFactoryCollection& value)
{
    if (!j.is_object()) {
        throw std::runtime_error("utility attributes must be an object");
    }
    const auto& registry =
        wmtk::components::configurator::transfer::TransferStrategyFactory::transfer_registry();
    for (const auto& [name, attr] : j.items()) {
        value[name] = registry.create(attr);
    }
}
} // namespace wmtk::components::configurator::transfer
