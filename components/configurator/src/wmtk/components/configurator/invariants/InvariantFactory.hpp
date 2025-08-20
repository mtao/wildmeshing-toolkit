#pragma once


#include "InvariantOptions.hpp"

#pragma once
#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>


namespace wmtk::invariants {
class Invariant;
}
namespace wmtk::components::multimesh {
class MeshCollection;
}

namespace wmtk::components::configurator {
class Configurator;
}

namespace wmtk::components::configurator::invariants {


class InvariantFactory
{
public:
    InvariantFactory();
    using InvariantCreatorFunc = std::function<
        std::shared_ptr<wmtk::invariants::Invariant>(Configurator&, const nlohmann::json& js)>;

    void add(const std::string& s, const InvariantCreatorFunc& f);

    void from_json(Configurator&, const nlohmann::json& js);

    std::shared_ptr<wmtk::invariants::Invariant>
    create(Configurator& config, std::string_view name, const nlohmann::json& js);
    std::shared_ptr<wmtk::invariants::Invariant> create(
        Configurator& config,
        const nlohmann::json& js);
    std::shared_ptr<wmtk::invariants::Invariant> get(const std::string& name);


private:
    std::map<std::string, InvariantCreatorFunc> m_invariant_functors;

    std::map<std::string, std::shared_ptr<wmtk::invariants::Invariant>> m_invariants;
};
} // namespace wmtk::components::configurator::invariants
