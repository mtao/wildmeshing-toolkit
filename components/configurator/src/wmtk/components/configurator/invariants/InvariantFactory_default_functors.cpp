#include "InvariantFactory_default_functors.hpp"
#include <wmtk/TriMesh.hpp>
#include <wmtk/invariants/Invariant.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/invariants/MultiMeshMapValidInvariant.hpp>
#include <wmtk/utils/Logger.hpp>
#include "../Configurator.hpp"
#include "InvariantFactory.hpp"
#include "InvariantOptions.hpp"
#include "wmtk/invariants/CannotMapSimplexInvariant.hpp"
#include "wmtk/invariants/EnvelopeInvariant.hpp"
#include "wmtk/invariants/InteriorEdgeInvariant.hpp"
#include "wmtk/invariants/InteriorSimplexInvariant.hpp"
#include "wmtk/invariants/InteriorVertexInvariant.hpp"
#include "wmtk/invariants/MaxEdgeLengthInvariant.hpp"
#include "wmtk/invariants/MinEdgeLengthInvariant.hpp"
#include "wmtk/invariants/MultiMeshLinkConditionInvariant.hpp"
#include "wmtk/invariants/RoundedInvariant.hpp"
#include "wmtk/invariants/SeparateSubstructuresInvariant.hpp"
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
template <typename T, typename MeshType = wmtk::Mesh, typename S = ThresholdInvariantParameters>
std::shared_ptr<wmtk::invariants::Invariant> default_add_attribute_threshold_invariant(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<TypedInvariantOptions<S>>();
    auto params = opts.get_parameters();

    auto attr = c.get_attribute(params.attribute);
    auto threshold = params.threshold(c.meshes());
    auto r = std::make_shared<T>(attr, threshold);
    return r;
}


} // namespace
void InvariantFactory::load_default_functors()
{
    add("link_condition",
        &default_add_mesh_invariant<wmtk::invariants::MultiMeshLinkConditionInvariant>,
        "Checks the link condition to only allow certain EdgeCollapses");

    add("separate_substructures",
        &default_add_mesh_invariant<wmtk::invariants::SeparateSubstructuresInvariant>,
        "Checks if two meshes are distinct after an edge collapse");

    add("multimesh_valid_map",
        &default_add_mesh_invariant<wmtk::invariants::MultiMeshMapValidInvariant>,
        "Verifies that any two simplices that an edge collapse merges into one remain distinct "
        "unless they are faces of a simplex that is being destroyed");

    add("interior_simplex",
        &default_add_mesh_invariant<wmtk::invariants::InteriorSimplexInvariant>);

    add(
        "collection",
        [](Configurator& c, const nlohmann::json& js) {
            invariants::TypedInvariantOptions<invariants::InvariantCollectionParameters> opts = js;
            auto params = opts.get_parameters();
            auto ic = std::make_shared<wmtk::invariants::InvariantCollection>(
                c.get_mesh(params.mesh_path));
            for (const auto& [name, inv] : params.invariants) {
                c.create_invariant(name, inv);
            }
            return ic;
        },
        "Represents a collection of invariants");

    add(
        "envelope",
        [](Configurator& c, const nlohmann::json& js) {
            invariants::TypedInvariantOptions<invariants::EnvelopeInvariantParameters> opts = js;
            auto params = opts.get_parameters();
            auto attr = c.get_attribute(params.attribute);
            double size = params.threshold(c.meshes());
            wmtk::attribute::MeshAttributeHandle env;

            if (params.envelope.empty()) {
                logger().debug(
                    "Envelope invariant didn't have an envelope attribute, assuming it was input "
                    "mesh "
                    "{}",
                    params.attribute);
                env = attr;
            } else {
                env = c.get_attribute(params.envelope);
            }

            auto r = std::make_shared<wmtk::invariants::EnvelopeInvariant>(env, size, attr);
            return r;
        },
        "Keeps an envelope within the bounds of an envelope");

    add("interior_edge", &default_add_mesh_invariant<wmtk::invariants::InteriorEdgeInvariant>);
    add("interior_vertex", &default_add_mesh_invariant<wmtk::invariants::InteriorVertexInvariant>);
    add("max_edge_length",
        &default_add_attribute_threshold_invariant<wmtk::invariants::MaxEdgeLengthInvariant>,
        "Only allows operations whose input attribute's edge length is below the provided "
        "threshold");
    add("min_edge_length",
        &default_add_attribute_threshold_invariant<wmtk::invariants::MinEdgeLengthInvariant>,
        "Only allows operations whose input attribute's edge length is above the provided "
        "threshold");
    add("is_rounded", &default_add_attribute_invariant<wmtk::invariants::RoundedInvariant>);


    add(
        "alias",
        [](Configurator& c, const nlohmann::json& js) {
            invariants::TypedInvariantOptions<invariants::AliasInvariantParameters> opts = js;
            auto params = opts.get_parameters();

            auto inv = c.get_invariant(params.name);
            auto ic = std::make_shared<wmtk::invariants::InvariantCollection>(inv->mesh());
            ic->add(inv);
            return ic;
        },
        "alias for another invariant according to the provided name");

    add(
        "cannot_map",
        [](Configurator& c, const nlohmann::json& js) {
            invariants::TypedInvariantOptions<invariants::CannotMapSimplexInvariantParameters>
                opts = js;
            auto params = opts.get_parameters();

            auto& m = c.get_mesh(params.mesh_path);
            auto& m2 = c.get_mesh(params.mapped_mesh_path);
            return std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                m,
                m2,
                get_primitive_type_from_id(params.simplex_dimension),
                params.invert);
        },
        "Only allows operations where the input simplex can be mapped to the given child mesh");


    // TODO: make simplex inversion a single name that is generic
    add("simplex_inversion",
        &default_add_attribute_invariant<wmtk::invariants::SimplexInversionInvariant<double>>,
        "Only allows operations that keep a positive orientation after");
    add("simplex_inversion_double",
        &default_add_attribute_invariant<wmtk::invariants::SimplexInversionInvariant<double>>,
        "Only allows operations that keep a positive orientation after for doubles");
    add("simplex_inversion_rational",
        &default_add_attribute_invariant<
            wmtk::invariants::SimplexInversionInvariant<wmtk::Rational>>,
        "Only allows operations that keep a positive orientation after for rationals");

    add("triangle_valence_improvement",
        &default_add_mesh_invariant<wmtk::invariants::ValenceImprovementInvariant, wmtk::TriMesh>,
        "Only enables operations for TriEdgeSplit that would improve the valence according to "
        "dzint's heuristic (without the boundary angle component)");

    // add("split", &default_add_invariant<wmtk::invariants::EdgeSplit,
    // EdgeSplitOptions>); add("collapse",
    //     &default_add_invariant<wmtk::invariants::EdgeCollapse, EdgeCollapseOptions>);
    // add("swap2", &default_add_invariant<
    //     wmtk::invariants::composite::TriEdgeSwap,
    //     EdgeSwapOptions,
    //     wmtk::TriMesh>);
}
} // namespace wmtk::components::configurator::invariants
