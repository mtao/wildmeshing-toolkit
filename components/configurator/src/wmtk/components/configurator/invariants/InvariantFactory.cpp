#include "InvariantFactory.hpp"
#include <fmt/ranges.h>
#include <fmt/std.h>
#include <spdlog/spdlog.h>
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>


namespace wmtk::components::configurator::invariants {


InvariantFactory::InvariantFactory()
{
    load_default_functors();
}
void InvariantFactory::from_json(Configurator& c, const nlohmann::json& js)
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
std::vector<std::string> InvariantFactory::known_invariant_functors() const
{
    auto tmp = std::views::transform(m_invariant_functors, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
}
std::vector<std::string> InvariantFactory::known_invariants() const
{
    auto tmp = std::views::transform(m_invariants, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
}

void InvariantFactory::add(
    const std::string& s,
    const InvariantCreatorFunc& f,
    std::string_view info)
{
    spdlog::warn(
        "Added invariant functor \"{}\" among {} available",
        s,
        known_invariant_functors());
    m_invariant_functors[s] = {f, std::string(info)};
}
std::shared_ptr<wmtk::invariants::Invariant> InvariantFactory::create(
    Configurator& config,
    const nlohmann::json& js)
{
    return create(config, js["name"].get<std::string>(), js);
}
std::shared_ptr<wmtk::invariants::Invariant>
InvariantFactory::create(Configurator& config, std::string_view name, const InvariantOptions& opts)
{
    const std::string& t = opts.type;
    spdlog::warn(
        "Creating invariant \"{}\" type \"{}\" among {} available",
        name,
        t,
        known_invariant_functors());
    const auto& [f, _] = m_invariant_functors.at(t);
    auto js = nlohmann::json(opts);
    auto r = f(config, js);
    m_invariants[std::string(name)] = {r, js};
    return r;
}

std::shared_ptr<wmtk::invariants::Invariant>
InvariantFactory::create(Configurator& config, std::string_view name, const nlohmann::json& js)
{
    return create(config, name, js.get<InvariantOptions>());
}
std::shared_ptr<wmtk::invariants::Invariant> InvariantFactory::get(const std::string& name)
{
    try {
        return m_invariants.at(name).first;
    } catch (const std::exception& e) {
        spdlog::warn("Added op functor \"{}\" among {} available", name, known_invariants());
        throw e;
    }
}

std::string_view InvariantFactory::get_name(const wmtk::invariants::Invariant& inv) const
{
    for (const auto& [name, my_inv] : m_invariants) {
        if (&inv == my_inv.first.get()) {
            return name;
        }
    }
    constexpr static std::string unknown = "unknown";
    return unknown;
}
} // namespace wmtk::components::configurator::invariants
