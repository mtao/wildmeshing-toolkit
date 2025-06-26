#include "CollapseAlternateFacetData.hpp"
#include <array>
#include <vector>
#include <wmtk/Mesh.hpp>
#include <wmtk/autogen/subgroup/convert.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/local_dart_action.hpp>
#include <wmtk/dart/utils/share_simplex.hpp>
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

    const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
    const wmtk::dart::Dart t_dart = sd.dart_from_tuple(t);

    auto ear_orientations = ear_actions(mesh_pt);
    for (auto& ear_orientation : ear_orientations) {
        ear_orientation = sd.product(ear_orientation, data.input.permutation());
    }


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


    // Find the action such that data.input = action * t_dart
    const int8_t action =
        wmtk::multimesh::utils::find_local_dart_action(mesh_pt, t_dart, data.input);
    auto map = [action, &sd, &data, ear_orientations](const size_t index) -> Tuple {
        const int8_t ear_orientation = ear_orientation[index];

        const int8_t in_ear_action =
            wmtk::dart::find_local_dart_action(sd, t_dart.permutation(), ear_orientation);

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
        return {};
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
