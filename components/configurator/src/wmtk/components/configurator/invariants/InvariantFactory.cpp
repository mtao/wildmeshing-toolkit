#include "InvariantFactory.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/invariants/Invariant.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include "../Configurator.hpp"
#include "InvariantOptions.hpp"
#include "wmtk/invariants/InteriorSimplexInvariant.hpp"
#include "wmtk/invariants/MultiMeshLinkConditionInvariant.hpp"
#include "wmtk/invariants/SimplexInversionInvariant.hpp"


namespace wmtk::components::configurator::invariants {

namespace {
template <typename T, typename S = MeshInvariantOptions, typename MeshType = wmtk::Mesh>
std::shared_ptr<wmtk::invariants::Invariant> default_add_mesh_invariant(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<S>();
    auto& m = c.get_mesh<MeshType>(opts.mesh_path);

    if (opts.on_every_mesh) {
        auto ic = std::make_shared<wmtk::invariants::InvariantCollection>(m);
        for (auto& child : m.get_multi_mesh_root().get_all_meshes()) {
            ic->add(std::make_shared<T>(*child));
        }
        return ic;
    } else {
        auto r = std::make_shared<T>(m);
        return r;
    }
}
template <typename T, typename S = AttributeInvariantOptions, typename MeshType = wmtk::Mesh>
std::shared_ptr<wmtk::invariants::Invariant> default_add_attribute_invariant(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<S>();

    auto attr = c.get_attribute(opts.attribute);
    auto r = std::make_shared<T>(attr);
    return r;
}

} // namespace

InvariantFactory::InvariantFactory()
{
    m_invariant_functors["link_condition"] =
        &default_add_mesh_invariant<wmtk::invariants::MultiMeshLinkConditionInvariant>;

    m_invariant_functors["interior_simplex"] =
        &default_add_mesh_invariant<wmtk::invariants::InteriorSimplexInvariant>;

    // TODO: make simplex inversion a single name that is generic
    m_invariant_functors["simplex_inversion"] =
        &default_add_attribute_invariant<wmtk::invariants::SimplexInversionInvariant<double>>;
    m_invariant_functors["simplex_inversion_double"] =
        &default_add_attribute_invariant<wmtk::invariants::SimplexInversionInvariant<double>>;
    m_invariant_functors["simplex_inversion_rational"] = &default_add_attribute_invariant<
        wmtk::invariants::SimplexInversionInvariant<wmtk::Rational>>;
    // m_invariant_functors["split"] = &default_add_invariant<wmtk::invariants::EdgeSplit,
    // EdgeSplitOptions>; m_invariant_functors["collapse"] =
    //     &default_add_invariant<wmtk::invariants::EdgeCollapse, EdgeCollapseOptions>;
    // m_invariant_functors["swap2"] = &default_add_invariant<
    //     wmtk::invariants::composite::TriEdgeSwap,
    //     EdgeSwapOptions,
    //     wmtk::TriMesh>;
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

void InvariantFactory::add(const std::string& s, const InvariantCreatorFunc& f)
{
    m_invariant_functors[s] = f;
}
std::shared_ptr<wmtk::invariants::Invariant> InvariantFactory::create(
    Configurator& config,
    const nlohmann::json& js)
{
    return create(config, js["name"].get<std::string>(), js);
}
std::shared_ptr<wmtk::invariants::Invariant>
InvariantFactory::create(Configurator& config, std::string_view name, const nlohmann::json& js)
{
    auto r = m_invariant_functors.at(js["type"])(config, js);
    m_invariants[std::string(name)] = r;
    return r;
}
std::shared_ptr<wmtk::invariants::Invariant> InvariantFactory::get(const std::string& name)
{
    return m_invariants.at(name);
}

std::string_view InvariantFactory::get_name(const wmtk::invariants::Invariant& op) const
{
    for (const auto& [name, my_op] : m_ops) {
        if (&op == my_op.get()) {
            return name;
        }
    }
    constexpr static std::string unknown = "unknown";
    return unknown;
}
} // namespace wmtk::components::configurator::invariants
