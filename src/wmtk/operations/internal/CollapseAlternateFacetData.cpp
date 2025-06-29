#include "CollapseAlternateFacetData.hpp"
#include <array>
#include <vector>
#include <wmtk/Mesh.hpp>
#include <wmtk/autogen/subgroup/convert.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include <wmtk/dart/local_dart_action.hpp>
#include <wmtk/dart/utils/edge_mirror.hpp>
#include <wmtk/dart/utils/largest_shared_subdart_size.hpp>
#include <wmtk/dart/utils/share_simplex.hpp>
#include <wmtk/dart/utils/subdart_maximal_action_to_face.hpp>
#include <wmtk/multimesh/utils/find_local_dart_action.hpp>
#include <wmtk/multimesh/utils/find_local_switch_sequence.hpp>
#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/primitive_range.hpp>
#include "ear_actions.hpp"
#include "wmtk/dart/SimplexDart.hpp"

namespace wmtk::operations::internal {
namespace {
constexpr auto sort_op = [](const CollapseAlternateFacetData::Data& a,
                            const CollapseAlternateFacetData::Data& b) -> bool {
    return a.input.global_id() < b.input.global_id();
};
constexpr auto sort_int_op = [](const CollapseAlternateFacetData::Data& value,
                                const int64_t& facet_id) -> bool {
    return value.input.global_id() < facet_id;
};
} // namespace
CollapseAlternateFacetData::CollapseAlternateFacetData(const Mesh& m, const Tuple& input_tuple)
{
    spdlog::info("Constructing");
    for (const auto& s : simplex::cofaces_single_dimension_tuples(
             m,
             simplex::Simplex::edge(input_tuple),
             m.top_simplex_type())) {
        add(m, s);
    }
}

void CollapseAlternateFacetData::add(const Mesh& m, const Tuple& input_tuple)
{
    m_data.emplace_back(m, input_tuple);
    // first tuple is different from the input by switching everything but vertex
    // second one is switch everything
}

void CollapseAlternateFacetData::sort()
{
    std::sort(m_data.begin(), m_data.end(), sort_op);
}


CollapseAlternateFacetData::CollapseAlternateFacetData() = default;
CollapseAlternateFacetData::~CollapseAlternateFacetData() = default;

auto CollapseAlternateFacetData::get_alternative_data_it(const int64_t& input_facet) const
    -> AltData::const_iterator
{
    AltData::const_iterator it;
    if (std::is_sorted(m_data.begin(), m_data.end(), sort_op)) {
        it = std::lower_bound(m_data.begin(), m_data.end(), input_facet, sort_int_op);
        auto end = m_data.cend();

        // if we found

        // fix case where the lower bound was not the target value
        if (it != end && it->input.global_id() != input_facet) {
            it = end;
        }
    } else {
        it = std::find(m_data.begin(), m_data.end(), input_facet);
    }
    return it;
}
auto CollapseAlternateFacetData::get_alternatives_data(const Tuple& t) const -> const Data&
{
    auto it = get_alternative_data_it(t.global_cid());
    assert(it != m_data.cend());
    return *it;
}
std::array<Tuple, 2> CollapseAlternateFacetData::get_alternatives(
    const PrimitiveType mesh_pt,
    const Tuple& t,
    const PrimitiveType simplex_dimension) const
{
    const auto& data = get_alternatives_data(t);
    spdlog::info(
        "internal raw alts data is {} {}",
        std::string(data.alts[0]),
        std::string(data.alts[1]));

    const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
    const wmtk::dart::Dart t_dart = sd.dart_from_tuple(t);

    const auto ear_orientations = ear_actions(mesh_pt);
    // for (auto& ear_orientation : ear_orientations) {
    //     ear_orientation = sd.product(data.input.permutation(), ear_orientation);
    // }


    // As input we recieve
    // t =
    //   x-----x-----x
    //   |    / \    |
    //   |   o   \   |
    //   |  /  o  \  |
    //   | /       \ |
    //   o-----------x
    //
    // Input for the edge operation was
    // input =
    //   x-----x-----x
    //   |    / \    |
    //   |   /   \   |
    //   |  /  o  \  |
    //   | /       \ |
    //   o-----o-----x

    // lopp =
    //   x-----o-----x
    //   |    / \    |
    //   |   o   \   |
    //   |  /  o  \  |
    //   | /       \ |
    //   x-----------x
    //
    // ropp =
    //   x-----o-----x
    //   |    / \    |
    //   |   /   o   |
    //   |  /  o  \  |
    //   | /       \ |
    //   x-----------x
    // lear =
    //   x-----o-----x
    //   | o  / \    |
    //   |   o   \   |
    //   |  /     \  |
    //   | /       \ |
    //   x-----------x
    // rear =
    //   x-----o-----x
    //   |    / \  o |
    //   |   /   o   |
    //   |  /     \  |
    //   | /       \ |
    //   x-----------x
    //
    //
    // we are trying to evaluate


    // Find the action such that data.input = action(t_dart)
    const int8_t relative_action =
        wmtk::multimesh::utils::find_local_dart_action(mesh_pt, data.input, t_dart);
    const PrimitiveType face_pt = mesh_pt - 1;
    spdlog::info("Relative action: {}", relative_action);
    auto map = [&](const size_t index) -> Tuple {
        const auto& alt = data.alts[index];
        if (alt.is_null()) {
            return {};
        }
        const int8_t ear_orientation = ear_orientations[index];

        const int8_t face_index = sd.simplex_index(ear_orientation, face_pt);

        int8_t in_ear_permutation = relative_action;
        if (int8_t largest_subdart_size = wmtk::dart::utils::largest_shared_subdart_size(
                mesh_pt,
                relative_action,
                face_pt,
                face_index);
            largest_subdart_size < get_primitive_type_id(simplex_dimension)) {
            in_ear_permutation = dart::utils::edge_mirror(sd, relative_action);

#if !defined(NDEBUG)
            largest_subdart_size = wmtk::dart::utils::largest_shared_subdart_size(
                mesh_pt,
                relative_action,
                face_pt,
                face_index);

            assert(largest_subdart_size < get_primitive_type_id(simplex_dimension));
#endif
        }

        const int8_t act = wmtk::dart::utils::subdart_maximal_action_to_face_action(
            mesh_pt,
            in_ear_permutation,
            face_pt,
            face_index);
        const int8_t moved_relative_action = sd.act(relative_action, act);
        spdlog::info(
            "Maximal subdart moved {} to {} with {}",
            relative_action,
            moved_relative_action,
            act);

        const int8_t in_ear_action =
            wmtk::dart::find_local_dart_action(sd, ear_orientation, moved_relative_action);
        spdlog::info(
            "Ear orientation {} got face index {}, relative action {}. Reached {} with {}",
            ear_orientation,
            face_index,
            in_ear_permutation,
            moved_relative_action,
            in_ear_action);

        spdlog::info("Alt here: {}", std::string(alt));
        const int8_t nbr_permutation = sd.act(alt.permutation(), in_ear_action);
        spdlog::info("{} x {} => {}", alt.permutation(), in_ear_action, nbr_permutation);
        const wmtk::dart::Dart d(alt.global_id(), nbr_permutation);
        spdlog::info("Sptting out {}", std::string(d));
        return sd.tuple_from_dart(d);
        /*

        if (wmtk::dart::subgroup::can_convert(mesh_pt, mesh_pt - 1, in_ear_action)) {
        }
        const PrimitiveType mappable_dart_dimension = a;
        if (transform.is_null() || mappable_dart_dimension < simplex_dimension) {
            return {};
        } else {
            int8_t projected_subdart = sd.convert(action, , mesh_pt - 1);
            int8_t mapped_dart = sd.product(tup.permutation(), action);
            const wmtk::dart::Dart d(tup.global_id(), mapped_dart);
            return sd.tuple_from_dart(d);
        }
        */
    };

    std::array<Tuple, 2> r{{map(0), map(1)}};

    return r;
}
Tuple CollapseAlternateFacetData::get_alternative(
    const PrimitiveType mesh_pt,
    const Tuple& t,
    const PrimitiveType pt) const
{
    // TODO: map to a valid face


    auto alts = get_alternatives(mesh_pt, t, pt);
    assert(!alts[0].is_null() || !alts[1].is_null());
    if (!alts[0].is_null()) {
        return alts[0];
    } else {
        return alts[1];
    }
    return {};
    //
}
} // namespace wmtk::operations::internal
