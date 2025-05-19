#include "configure_collapse.hpp"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/invariants/CannotMapSimplexInvariant.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/invariants/MaxEdgeLengthInvariant.hpp>
#include <wmtk/invariants/InteriorSimplexInvariant.hpp>
#include <wmtk/invariants/MultiMeshLinkConditionInvariant.hpp>
#include <wmtk/invariants/MultiMeshMapValidInvariant.hpp>
#include <wmtk/invariants/SeparateSubstructuresInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/invariants/uvEdgeInvariant.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/operations/attribute_new/CollapseNewAttributeStrategy.hpp>
#include "../IsotropicRemeshingOptions.hpp"
namespace wmtk::components::isotropic_remeshing::internal {

std::shared_ptr<wmtk::invariants::InvariantCollection> collapse_core_invariants(
    Mesh& m,
    const IsotropicRemeshingOptions& options)
{
    auto& root = m.get_multi_mesh_root();
    auto ic_root = std::make_shared<wmtk::invariants::InvariantCollection>(root);
    auto invariant_link_condition =
        std::make_shared<wmtk::invariants::MultiMeshLinkConditionInvariant>(root);

    auto invariant_mm_map = std::make_shared<MultiMeshMapValidInvariant>(root);
    ic_root->add(invariant_link_condition);
    ic_root->add(invariant_mm_map);
    if (options.separate_substructures) {
        auto invariant_separate_substructures =
            std::make_shared<wmtk::invariants::SeparateSubstructuresInvariant>(root);
        ic_root->add(invariant_separate_substructures);
    }
    return ic_root;
}

std::shared_ptr<wmtk::invariants::InvariantCollection> collapse_invariants(
    Mesh& m,
    const IsotropicRemeshingOptions& options)
{
    auto ic_root = collapse_core_invariants(m, options);

    auto ic = std::make_shared<wmtk::invariants::InvariantCollection>(m);
    const std::optional<attribute::MeshAttributeHandle>& position_for_inversion =
        options.inversion_position_attribute;


    if (position_for_inversion) {
        ic->add(std::make_shared<SimplexInversionInvariant<double>>(
            position_for_inversion.value().mesh(),
            position_for_inversion.value().as<double>()));
    }

    ic->add(ic_root);
    const double length_min = (4. / 5.) * options.get_absolute_length();
    auto invariant_max_edge_length = std::make_shared<MaxEdgeLengthInvariant>(
        options.position_attribute.mesh(),
        options.position_attribute.as<double>(),
        length_min * length_min);
    ic->add(invariant_max_edge_length);

    /*

    // hack for uv
    if (options.fix_uv_seam) {
        assert(
            options.other_position_attributes.size() ==
            1); // this only works with a 2d pos + uv mesh for now

        ic->add(std::make_shared<invariants::uvEdgeInvariant>(
            m,
            options.other_position_attributes.front().mesh()));
    }


    if (options.lock_boundary && !options.use_for_periodic) {
        auto invariant_interior_edge = std::make_shared<invariants::InvariantCollection>(m);
        ic->add(invariant_interior_edge);
    }
    */
    return ic;
}

void configure_collapse(
    operations::EdgeCollapse& ec,
    Mesh& m,
    const IsotropicRemeshingOptions& options)
{
    auto invars = collapse_invariants(m, options);
    ec.add_invariant(invars);


    const auto positions = options.all_positions();

    if (options.lock_boundary && !options.use_for_periodic) {
        assert(false);
        // set collapse towards boundary
        for (auto& p : positions) {
            auto tmp = std::make_shared<operations::CollapseNewAttributeStrategy<double>>(p);
            tmp->set_strategy(operations::CollapseBasicStrategy::Mean);
            tmp->set_simplex_predicate(operations::BasicSimplexPredicate::IsInterior);
            ec.set_new_attribute_strategy(p, tmp);
        }
    } else if (options.use_for_periodic) {
        assert(false); // TODO: make fusion simplex invariant
        ec.add_invariant(
            std::make_shared<wmtk::invariants::FusionEdgeInvariant>(m, m.get_multi_mesh_root()));
        for (auto& p : positions) {
            ec.set_new_attribute_strategy(p, operations::CollapseBasicStrategy::Mean);
        }
    } else {
        for (auto& p : positions) {
            ec.set_new_attribute_strategy(p, operations::CollapseBasicStrategy::Mean);
        }
    }

    if (options.mesh_collection != nullptr) {
        if (!options.static_meshes.empty()) {
            std::vector<std::shared_ptr<Mesh>> static_meshes;
            for (const auto& mesh_name : options.static_meshes) {
                auto& mesh2 = options.mesh_collection->get_mesh(mesh_name);
                static_meshes.emplace_back(mesh2.shared_from_this());
                // disallow operations that collapse a mapped simplex
                ec.add_invariant(std::make_shared<invariants::CannotMapSimplexInvariant>(m, mesh2));
            }
            auto strat_ptr_const = ec.get_new_attribute_strategy(options.position_attribute);
            auto strat_ptr =
                std::const_pointer_cast<wmtk::operations::BaseCollapseNewAttributeStrategy>(
                    strat_ptr_const);
            strat_ptr->set_simplex_predicate(
                [static_meshes, &m](const simplex::Simplex& s) -> bool {
                    simplex::Simplex sv = simplex::Simplex(PrimitiveType::Vertex, s.tuple());
                    for (const auto& mptr : static_meshes) {
                        bool mappable = m.can_map(*mptr, sv);
                        if (mappable) {
                            return true;
                        }
                    }
                    return false;
                    return {};
                });
        }
    }
    for (const auto& mptr : m.get_all_child_meshes()) {
        auto& mesh2 = *mptr;
        switch (mesh2.top_simplex_type()) {
        case wmtk::PrimitiveType::Edge:
            /*
            ec.add_invariant(std::make_shared<wmtk::invariants::InteriorSimplexInvariant>(
                mesh2,
                wmtk::PrimitiveType::Edge));
        */
            /*
            spdlog::warn("Making the swap always go false");
            swap->add_invariant(std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                mesh,
                mesh2,
                wmtk::PrimitiveType::Edge,
                false));
            swap->add_invariant(std::make_shared<wmtk::invariants::CannotMapSimplexInvariant>(
                mesh,
                mesh2,
                wmtk::PrimitiveType::Edge,
                true));
            */
            // ec.add_invariant(std::make_shared<invariants::CannotMapSimplexInvariant>(
            //     m,
            //     mesh2,
            //     wmtk::PrimitiveType::Edge,
            //     false));
            break;
        case wmtk::PrimitiveType::Triangle: break;
        case wmtk::PrimitiveType::Vertex: break;
        case wmtk::PrimitiveType::Tetrahedron:
        default: assert(false);
        }
    }

    for (const auto& attr : options.pass_through_attributes) {
        ec.set_new_attribute_strategy(attr);
    }
}
} // namespace wmtk::components::isotropic_remeshing::internal
