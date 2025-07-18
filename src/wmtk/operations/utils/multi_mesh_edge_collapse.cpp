#include "multi_mesh_edge_collapse.hpp"
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/multimesh/MultiMeshSimplexEventVisitor.hpp>
#include <wmtk/multimesh/MultiMeshSimplexVisitor.hpp>
#include <wmtk/multimesh/MultiMeshVisitor.hpp>
#include <wmtk/multimesh/operations/extract_operation_tuples.hpp>
#include <wmtk/operations/attribute_new/CollapseNewAttributeStrategy.hpp>
#include <wmtk/operations/utils/MultiMeshEdgeCollapseFunctor.hpp>
#include <wmtk/operations/utils/UpdateEdgeOperationMultiMeshMapFunctor.hpp>
#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>

#include <wmtk/TriMesh.hpp>

namespace wmtk::operations::utils {

std::shared_ptr<invariants::InvariantCollection> multimesh_edge_collapse_invariants(const Mesh& m)
{
    auto invariants = std::make_shared<invariants::InvariantCollection>(m);
    //*invariants = basic_multimesh_invariant_collection(m, PrimitiveType::Edge);
    return invariants;
}

CollapseReturnData multi_mesh_edge_collapse(
    Mesh& mesh,
    const simplex::NavigatableSimplex& t,
    const std::vector<std::shared_ptr<const operations::BaseCollapseNewAttributeStrategy>>&
        new_attr_strategies)
{
    multimesh::MultiMeshSimplexVisitor visitor(
        std::integral_constant<int64_t, 1>{}, // specify that this runs over edges
        MultiMeshEdgeCollapseFunctor{});
#if defined(MTAO_CONSTANTLY_VERIFY_MESH)
    assert(mesh.is_connectivity_valid());
#endif
    visitor.execute_from_root(mesh, t);

#if defined(MTAO_CONSTANTLY_VERIFY_MESH)
    assert(mesh.is_connectivity_valid());
#endif
    multimesh::MultiMeshSimplexEventVisitor event_visitor(visitor);
    event_visitor.run_on_nodes(UpdateEdgeOperationMultiMeshMapFunctor{});
#if defined(MTAO_CONSTANTLY_VERIFY_MESH)
    assert(mesh.is_connectivity_valid());
#endif

    auto cache = visitor.take_cache();

    auto tuples = wmtk::multimesh::operations::extract_operation_in_out(cache);
    auto update_attributes = [&](auto&& m) {
        using T = std::remove_reference_t<decltype(m)>;
        if constexpr (!std::is_const_v<T>) {
            for (const auto& collapse_ptr : new_attr_strategies) {
                if (&m == &collapse_ptr->mesh()) {
                    collapse_ptr->update(m, cache, tuples);
                }
            }
        }
    };

#if defined(MTAO_CONSTANTLY_VERIFY_MESH)
    assert(mesh.is_connectivity_valid());
#endif
    multimesh::MultiMeshVisitor(update_attributes).execute_from_root(mesh);

#if defined(MTAO_CONSTANTLY_VERIFY_MESH)
    assert(mesh.is_connectivity_valid());
#endif
    return cache;
}
std::vector<simplex::Simplex> multi_mesh_edge_collapse_with_modified_simplices(
    Mesh& mesh,
    const simplex::Simplex& simplex,
    const std::vector<std::shared_ptr<const operations::BaseCollapseNewAttributeStrategy>>&
        new_attr_strategies)
{
    simplex::NavigatableSimplex nsimplex(mesh, simplex);
    auto return_data =
        operations::utils::multi_mesh_edge_collapse(mesh, nsimplex, new_attr_strategies);


    if (mesh.is_free()) {
        return std::vector<simplex::Simplex>{1};
    }

    auto func = [&mesh](const auto& rt) -> std::vector<simplex::Simplex> {
        return {simplex::Simplex::vertex(mesh, rt.m_output_tuple)};
    };


    if (return_data.has_variant(mesh, nsimplex)) {
        return std::visit(func, return_data.get_variant(mesh, nsimplex));
    }
    auto candidates = top_dimension_cofaces(mesh, simplex);

    for (const auto& c : candidates) {
        simplex::NavigatableSimplex nc(mesh, c);
        if (return_data.has_variant(mesh, nc)) {
            return std::visit(func, return_data.get_variant(mesh, nc));
        }
    }

    assert(return_data.has_variant(mesh, nsimplex));

    // return std::visit(
    //     [&mesh](const auto& rt) -> std::vector<simplex::Simplex> {
    //         return {simplex::Simplex::vertex(mesh, rt.m_output_tuple)};
    //     },
    //     return_data.get_variant(mesh, simplex));
    return {};
}
} // namespace wmtk::operations::utils
