#include "TransferStrategyFactoryRegistry.hpp"
#include <nlohmann/json.hpp>
#include <ranges>
#include <wmtk/utils/Logger.hpp>
#include "TransferStrategyFactory.hpp"

namespace wmtk::components::configurator::transfer {
bool TransferStrategyFactoryRegistry::has(const std::string_view& str) const
{
    return m_map.find(std::string(str)) != m_map.end();
}

std::shared_ptr<TransferStrategyFactory> TransferStrategyFactoryRegistry::create(
    const nlohmann::json& js) const
{
    return create(js.at("type").get<std::string>(), js);
}

std::shared_ptr<TransferStrategyFactory> TransferStrategyFactoryRegistry::create(
    const std::string_view& name,
    const nlohmann::json& js) const
{
    try {
        return m_map.at(std::string(name))(js);
    } catch (const std::out_of_range& err) {
        auto r = m_map | std::views::transform([](const auto& s) { return s.first; });
        wmtk::logger().error(
            "Could not find \"{}\" in transfer registry, which had [{}] available",
            name,
            r);

        throw err;
    } catch (const std::exception& err) {
        wmtk::logger().error("Failed to create transfer \"{}\" got error [{}]", name, err.what());
        throw err;
    }
}
} // namespace wmtk::components::configurator::transfer
