#include "TransferStrategyFactoryRegistry.hpp"
#include <nlohmann/json.hpp>
#include <ranges>
#include <wmtk/utils/Logger.hpp>
#include "TransferStrategyFactory.hpp"
#include "TransferStrategyOptions.hpp"

namespace wmtk::components::configurator::transfer {
bool TransferStrategyFactoryRegistry::has(const std::string_view& str) const
{
    return m_map.find(std::string(str)) != m_map.end();
}

std::shared_ptr<TransferStrategyFactory> TransferStrategyFactoryRegistry::create(
    const TransferStrategyOptions& settings) const
{
    return create(settings.type, settings);
}

std::shared_ptr<TransferStrategyFactory> TransferStrategyFactoryRegistry::create(
    const std::string_view& type,
    const TransferStrategyOptions& settings) const
{
    try {
        return m_map.at(std::string(type))(settings);
    } catch (const std::out_of_range& err) {
        auto r = m_map | std::views::transform([](const auto& s) { return s.first; });
        wmtk::logger().error(
            "Could not find \"{}\" in transfer registry, which had [{}] available",
            type,
            r);

        throw err;
    } catch (const std::exception& err) {
        wmtk::logger().error("Failed to create transfer \"{}\" got error [{}]", type, err.what());
        throw err;
    }
}

std::vector<std::string> TransferStrategyFactoryRegistry::names() const
{
    auto tmp = std::views::transform(m_map, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));

    return ret;
}
} // namespace wmtk::components::configurator::transfer
