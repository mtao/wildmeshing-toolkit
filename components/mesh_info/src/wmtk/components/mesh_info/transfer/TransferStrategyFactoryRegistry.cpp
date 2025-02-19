#include "TransferStrategyFactoryRegistry.hpp"
#include "TransferStrategyFactory.hpp"
#include <nlohmann/json.hpp>

namespace wmtk::components::mesh_info::transfer {
bool TransferStrategyFactoryRegistry::has(const std::string_view& str) const
{
    return m_map.find(std::string(str)) == m_map.end();
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
    return m_map.at(std::string(name))(js);
}
} // namespace wmtk::components::mesh_info::transfer
