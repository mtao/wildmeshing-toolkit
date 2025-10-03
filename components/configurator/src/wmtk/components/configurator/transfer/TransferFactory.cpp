
#include "TransferFactory.hpp"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include "../Configurator.hpp"
#include "TransferStrategyFactory.hpp"
#include "TransferStrategyOptions.hpp"
namespace wmtk::components::configurator::transfer {

TransferFactory::TransferFactory()
    : m_registry(TransferStrategyFactory::transfer_registry_ptr())
{}

void TransferFactory::from_json(Configurator& c, const nlohmann::json& js)
{
    if (js.is_array()) {
        for (const auto& j : js) {
            create(c, j);
        }
    } else {
        assert(js.is_object());
        for (const auto& [n, j] : js.items()) {
            create(c, n, j);
        }
    }
}
std::vector<std::string> TransferFactory::known_transfer_functors() const
{
    return m_registry->names();
    /*
    auto tmp = std::views::transform(m_transfer_functors, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
    */
}
std::vector<std::string> TransferFactory::known_transfers() const
{
    auto tmp =
        std::views::transform(m_operation_transfers, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
}

// void TransferFactory::add(const std::string& s, const TransferCreatorFunc& f, std::string_view
// info)
//{
//     spdlog::warn(
//         "Added transfer functor \"{}\" among {} available",
//         s,
//         known_transfer_functors());
//     m_transfer_functors[s] = {f, std::string(info)};
// }
auto TransferFactory::create(Configurator& config, const nlohmann::json& js)
    -> std::shared_ptr<transfer_strategy_type>
{
    return create(config, js["name"].get<std::string>(), js);
}
auto TransferFactory::create(
    Configurator& config,
    std::string_view name,
    const TransferStrategyOptions& opts) -> std::shared_ptr<transfer_strategy_type>
{
    try {
        auto [child, oopt] = m_operation_transfers.at(std::string(name));
        if (bool(child)) {
            assert(oopt == opts);
            return child;
        }
    } catch (const std::exception& e) {
    }

    const std::string& t = opts.type;
    spdlog::warn(
        "Creating transfer \"{}\" type \"{}\" among {} available",
        name,
        t,
        known_transfer_functors());

    auto func = m_registry->create(opts);

    auto r = func->create(config.meshes());
    m_operation_transfers[std::string(name)] = {r, opts};
    return r;
}

auto TransferFactory::create(Configurator& config, std::string_view name, const nlohmann::json& js)
    -> std::shared_ptr<transfer_strategy_type>
{
    return create(config, name, js.get<TransferStrategyOptions>());
}
auto TransferFactory::get(const std::string& name) -> std::shared_ptr<transfer_strategy_type>
{
    try {
        return m_operation_transfers.at(name).first;
    } catch (const std::exception& e) {
        spdlog::warn("Added op functor \"{}\" among {} available", name, known_transfers());
        throw e;
    }
}

std::string_view TransferFactory::get_name(const transfer_strategy_type& inv) const
{
    for (const auto& [name, my_inv] : m_operation_transfers) {
        if (&inv == my_inv.first.get()) {
            return name;
        }
    }
    constexpr static std::string unknown = "unknown";
    return unknown;
}

} // namespace wmtk::components::configurator::transfer
