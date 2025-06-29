#include "CollapseAlternateFacetOptionData.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/autogen/local_switch_tuple.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include <wmtk/dart/local_dart_action.hpp>
#include <wmtk/dart/utils/apply_simplex_involution.hpp>
#include <wmtk/dart/utils/edge_mirror.hpp>
#include <wmtk/dart/utils/get_simplex_involution.hpp>
#include <wmtk/dart/utils/largest_shared_subdart_size.hpp>
#include <wmtk/dart/utils/subdart_maximal_action_to_face.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "ear_actions.hpp"
namespace wmtk::operations::internal {

CollapseAlternateFacetOptionData::CollapseAlternateFacetOptionData(
    const Mesh& m,
    const dart::SimplexDart& sd,
    const Tuple& input_tuple)
    : input(sd.dart_from_tuple(input_tuple))
    , alts({{left_switches(m, input_tuple), right_switches(m, input_tuple)}})
    , local_boundary_indices({{
          input_tuple.local_id(m.top_simplex_type() - 1),
          m.switch_tuple(input_tuple, PrimitiveType::Vertex).local_id(m.top_simplex_type() - 1),
      }})
{}


CollapseAlternateFacetOptionData::CollapseAlternateFacetOptionData(
    const Mesh& m,
    const Tuple& input_tuple)
    : CollapseAlternateFacetOptionData(
          m,
          dart::SimplexDart::get_singleton(m.top_simplex_type()),
          input_tuple)
{}

auto CollapseAlternateFacetOptionData::left_switches(const Mesh& m, const Tuple& t) const -> Dart
{
    const PrimitiveType mesh_type = m.top_simplex_type();
    return get_neighbor_action(m, t, left_ear_action(mesh_type));
}
auto CollapseAlternateFacetOptionData::right_switches(const Mesh& m, const Tuple& t) const -> Dart
{
    const PrimitiveType mesh_type = m.top_simplex_type();
    return get_neighbor_action(m, t, right_ear_action(mesh_type));
}

auto CollapseAlternateFacetOptionData::get_neighbor_action(
    const Mesh& m,
    const Tuple& edge_tuple,
    int8_t local_action) const -> Dart
{
    const PrimitiveType mesh_type = m.top_simplex_type();
    const PrimitiveType boundary_type = mesh_type - 1;
    Tuple r = wmtk::autogen::local_switch_tuple(mesh_type, edge_tuple, local_action);
    Dart d;
    if (!m.is_boundary(boundary_type, r)) {
        const auto& sd = dart::SimplexDart::get_singleton(m.top_simplex_type());
        int8_t source_orientation = sd.permutation_index_from_tuple(r);
        r = m.switch_tuple(r, m.top_simplex_type());
        d = sd.dart_from_tuple(r);
        int8_t& target_orientation = d.permutation();

        target_orientation = dart::utils::get_simplex_involution(
            mesh_type,
            source_orientation,
            mesh_type,
            target_orientation);
    }

    return d;
}

auto CollapseAlternateFacetOptionData::map_permutation_to_alt(
    const wmtk::dart::SimplexDart& sd,
    const int8_t p,
    int8_t index) const -> Dart
{
    const wmtk::dart::Dart& transform = alts[index];
    assert(!transform.is_null());
    const int8_t& local_boundary_index = local_boundary_indices[index];
    const PrimitiveType mesh_pt = sd.simplex_type();
    const PrimitiveType face_pt = mesh_pt - 1;


    int8_t to_face = wmtk::dart::utils::subdart_maximal_action_to_face_action(
        mesh_pt,
        p,
        face_pt,
        local_boundary_index);
    int8_t max_subdart = sd.act(p, to_face);
    int8_t ret_p =
        wmtk::dart::utils::apply_simplex_involution(mesh_pt, mesh_pt, transform.permutation(), p);

    return Dart(transform.global_id(), ret_p);
}

auto CollapseAlternateFacetOptionData::map_permutation_to_alt(
    const wmtk::dart::SimplexDart& sd,
    const int8_t permutation) const -> Dart
{
    const bool left_is_boundary = alts[0].is_null();
    const bool right_is_boundary = alts[1].is_null();

    int8_t index;
    const PrimitiveType mesh_pt = sd.simplex_type();
    const PrimitiveType face_pt = mesh_pt - 1;
    if (left_is_boundary && right_is_boundary) {
        int8_t left_largest_subdart = wmtk::dart::utils::largest_shared_subdart_size(
            mesh_pt,
            permutation,
            face_pt,
            local_boundary_indices[0]);
        int8_t right_largest_subdart = wmtk::dart::utils::largest_shared_subdart_size(
            mesh_pt,
            permutation,
            face_pt,
            local_boundary_indices[1]);
        if (left_largest_subdart >= right_largest_subdart) {
            index = 1;
        } else {
            index = 0;
        }

    } else {
        assert(left_is_boundary || right_is_boundary);
        index = alts[0].is_null() ? 1 : 0;
    }
    assert(!alts[index].is_null());
    return map_permutation_to_alt(sd, permutation, index);
}
dart::Dart CollapseAlternateFacetOptionData::map_dart_to_alt(
    const wmtk::dart::SimplexDart& sd,
    const Dart& d) const
{
    return map_permutation_to_alt(sd, d.permutation());
}
} // namespace wmtk::operations::internal
