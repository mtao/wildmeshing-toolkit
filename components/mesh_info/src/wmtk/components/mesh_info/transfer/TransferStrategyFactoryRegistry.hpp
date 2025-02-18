#pragma once
#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>

namespace wmtk::components::mesh_info::transfer {
class TransferStrategyFactory;

// registry holding the
class TransferStrategyFactoryRegistry
{
public:
    bool has(const std::string_view& name) const;
    template <typename Type>
    void register_transfer(const std::string_view& name);
    std::shared_ptr<TransferStrategyFactory> create(
        const std::string_view& name,
        const nlohmann::json&) const;

private:
    std::map<
        std::string,
        std::function<std::shared_ptr<TransferStrategyFactory>(const nlohmann::json&)>>
        m_map;
};

template <typename Type>
void TransferStrategyFactoryRegistry::register_transfer(const std::string_view& name)
{
    if (has(name)) {
        m_map.emplace(name, [](const nlohmann::json& js) {
            auto t = std::make_shared<Type>();
            t->from_json(js);
            return t;
        });
    }
}
} // namespace wmtk::components::mesh_info::transfer

