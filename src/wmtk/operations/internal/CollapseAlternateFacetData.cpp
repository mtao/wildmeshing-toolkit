#include "CollapseAlternateFacetData.hpp"
#include <fmt/ranges.h>
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

CollapseAlternateFacetData::CollapseAlternateFacetData(
    const Mesh& m,
    const Tuple& input_tuple,
    const std::vector<Tuple>& local_input_tuples)
{
    for (const auto& s : local_input_tuples) {
        add(m, s);
    }
}

CollapseAlternateFacetData::CollapseAlternateFacetData(const Mesh& m, const Tuple& input_tuple)
    : CollapseAlternateFacetData(
          m,
          input_tuple,
          simplex::cofaces_single_dimension_tuples(
              m,
              simplex::Simplex::edge(input_tuple),
              m.top_simplex_type()))
{}

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
    spdlog::info("looking for facet {}");
    for( const auto& t: m_data) {
        spdlog::info("Option: {} {}", std::string(t.input), fmt::join(t.alts,","));
    }
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
    return get_alternatives(mesh_pt, t);
}
std::array<Tuple, 2> CollapseAlternateFacetData::get_alternatives(
    const PrimitiveType mesh_pt,
    const Tuple& t) const
{
    const auto& data = get_alternatives_data(t);
    spdlog::info(
        "internal raw alts data is {} {}",
        std::string(data.alts[0]),
        std::string(data.alts[1]));

    const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
    const wmtk::dart::Dart t_dart = sd.dart_from_tuple(t);
    auto run = [&](int8_t index) {
        auto d = data.map_permutation_to_alt(sd, t_dart.permutation(), index);
        return sd.tuple_from_dart(d);
    };

    return std::array<Tuple, 2>{{run(0), run(1)}};
}
Tuple CollapseAlternateFacetData::get_alternative(
    const PrimitiveType mesh_pt,
    const Tuple& t,
    const PrimitiveType pt) const
{
    return get_alternative(mesh_pt, t);
}

Tuple CollapseAlternateFacetData::get_alternative(const PrimitiveType mesh_pt, const Tuple& t) const
{
    // TODO: map to a valid face

    const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
    const auto& data = get_alternatives_data(t);
    return sd.tuple_from_dart(data.map_dart_to_alt(sd, sd.dart_from_tuple(t)));

    //
}
} // namespace wmtk::operations::internal
