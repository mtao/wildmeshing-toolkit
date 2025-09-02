#include <spdlog/spdlog.h>
#include <wmtk/TriMesh.hpp>
#include <wmtk/invariants/Invariant.hpp>
#include <wmtk/invariants/MultiMeshMapValidInvariant.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include "../Configurator.hpp"
#include "InvariantFactory.hpp"
#include "InvariantOptions.hpp"
#include "wmtk/invariants/InteriorSimplexInvariant.hpp"
#include "wmtk/invariants/MultiMeshLinkConditionInvariant.hpp"
#include "wmtk/invariants/SimplexInversionInvariant.hpp"
#include "wmtk/invariants/ValenceImprovementInvariant.hpp"


namespace wmtk::components::configurator::invariants {
namespace {
template <typename T, typename MeshType = wmtk::Mesh, typename S = MeshInvariantParameters>
std::shared_ptr<wmtk::invariants::Invariant> default_add_mesh_invariant(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<TypedInvariantOptions<S>>();
    auto params = opts.get_parameters();
    MeshType& m = c.get_mesh<MeshType>(params.mesh_path);

    /*
    if (params.on_every_mesh) {
        auto ic = std::make_shared<wmtk::invariants::InvariantCollection>(m);
        for (auto& child : m.get_multi_mesh_root().get_all_meshes()) {
            ic->add(std::make_shared<T>(*child));
        }
        return ic;
    } else {
    */
    auto r = std::make_shared<T>(m);
    return r;
    //}
}
template <typename T, typename MeshType = wmtk::Mesh, typename S = AttributeInvariantParameters>
std::shared_ptr<wmtk::invariants::Invariant> default_add_attribute_invariant(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<TypedInvariantOptions<S>>();
    auto params = opts.get_parameters();

    auto attr = c.get_attribute(params.attribute);
    auto r = std::make_shared<T>(attr);
    return r;
}

} // namespace
void InvariantFactory::load_default_functors()
{
    m_invariant_functors["link_condition"] =
        &default_add_mesh_invariant<wmtk::invariants::MultiMeshLinkConditionInvariant>;

    m_invariant_functors["multimesh_valid_map"] =
        &default_add_mesh_invariant<wmtk::invariants::MultiMeshMapValidInvariant>;

    m_invariant_functors["interior_simplex"] =
        &default_add_mesh_invariant<wmtk::invariants::InteriorSimplexInvariant>;

    m_invariant_functors["collection"] = [](Configurator& c, const nlohmann::json& js) {
        invariants::TypedInvariantOptions<invariants::InvariantCollectionParameters> opts = js;
        auto params = opts.get_parameters();
        auto ic =
            std::make_shared<wmtk::invariants::InvariantCollection>(c.get_mesh(params.mesh_path));
        for (const auto& [name, inv] : params.invariants) {
            c.create_invariant(name, inv);
        }


        return ic;
    };
    m_invariant_functors["alias"] = [](Configurator& c, const nlohmann::json& js) {
        invariants::TypedInvariantOptions<invariants::AliasInvariantParameters> opts = js;
        auto params = opts.get_parameters();

        auto inv = c.get_invariant(params.name);
        auto ic = std::make_shared<wmtk::invariants::InvariantCollection>(inv->mesh());
        ic->add(inv);
        return ic;
    };


    // TODO: make simplex inversion a single name that is generic
    m_invariant_functors["simplex_inversion"] =
        &default_add_attribute_invariant<wmtk::invariants::SimplexInversionInvariant<double>>;
    m_invariant_functors["simplex_inversion_double"] =
        &default_add_attribute_invariant<wmtk::invariants::SimplexInversionInvariant<double>>;
    m_invariant_functors["simplex_inversion_rational"] = &default_add_attribute_invariant<
        wmtk::invariants::SimplexInversionInvariant<wmtk::Rational>>;

    m_invariant_functors["triangle_valence_improvement"] =
        &default_add_mesh_invariant<wmtk::invariants::ValenceImprovementInvariant, wmtk::TriMesh>;

    // m_invariant_functors["split"] = &default_add_invariant<wmtk::invariants::EdgeSplit,
    // EdgeSplitOptions>; m_invariant_functors["collapse"] =
    //     &default_add_invariant<wmtk::invariants::EdgeCollapse, EdgeCollapseOptions>;
    // m_invariant_functors["swap2"] = &default_add_invariant<
    //     wmtk::invariants::composite::TriEdgeSwap,
    //     EdgeSwapOptions,
    //     wmtk::TriMesh>;
}
} // namespace wmtk::components::configurator::invariants
