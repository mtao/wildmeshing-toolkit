#include "SplitAlternateFacetData.hpp"
#include <fmt/ranges.h>
#include <algorithm>
#include <array>
#include <cassert>
#include <vector>
#include <wmtk/Mesh.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_cofaces.hpp>
#include <wmtk/operations/EdgeOperationData.hpp>
#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include "ear_actions.hpp"
namespace wmtk::operations::internal {
namespace {
auto sort_op = [](const SplitAlternateFacetData::Data& a,
                  const SplitAlternateFacetData::Data& b) -> bool {
    return a.input.global_id() < b.input.global_id();
};

auto sort_int_op = [](const SplitAlternateFacetData::Data& value, const int64_t& facet_id) -> bool {
    return value.input.global_id() < facet_id;
};
} // namespace
SplitAlternateFacetData::SplitAlternateFacetData(
    Mesh& m,
    const Tuple& input_tuple,
    const std::vector<Tuple>& local_input_tuples)
{
    const std::vector<int64_t> nids= EdgeOperationData::request_simplex_indices(m, m.top_simplex_type(), 2 * local_input_tuples.size());
    for(size_t j = 0; j < local_input_tuples.size(); ++j) {
        size_t off = 2 * j;
        add_facet(m, local_input_tuples[j], std::array<int64_t,2>{{nids[off],nids[off+1]}});
    }
}

SplitAlternateFacetData::SplitAlternateFacetData(Mesh& m, const Tuple& input_tuple)
    : SplitAlternateFacetData(
          m,
          input_tuple,
          simplex::cofaces_single_dimension_tuples(
              m,
              simplex::Simplex::edge(input_tuple),
              m.top_simplex_type()))
{

}


void SplitAlternateFacetData::sort()
{
    std::sort(m_facet_maps.begin(), m_facet_maps.end(), sort_op);
}

// assumes the split cell map has been sorted
auto SplitAlternateFacetData::get_alternative_facets_it(const int64_t& input_cell) const
    -> AltData::const_iterator
{
    AltData::const_iterator it;
    if (std::is_sorted(m_facet_maps.begin(), m_facet_maps.end(), sort_op)) {
        it = std::lower_bound(m_facet_maps.begin(), m_facet_maps.end(), input_cell, sort_int_op);
        auto end = m_facet_maps.cend();
        // fix case where the lower bound was not the target value
        if (it != end && it->input.global_id() != input_cell) {
            it = end;
        }
    } else {
        it = std::find(m_facet_maps.begin(), m_facet_maps.end(), input_cell);
    }
    return it;
}
auto SplitAlternateFacetData::add_facet(wmtk::Mesh& mesh, const wmtk::Tuple& edge_tuple)
    -> const Data&
{
    const PrimitiveType pt = mesh.top_simplex_type();
    const std::vector<int64_t> new_eids = EdgeOperationData::request_simplex_indices(mesh, pt, 2);
    std::array<int64_t, 2> dat;
    std::copy(new_eids.begin(), new_eids.end(), dat.begin());
    return add_facet(mesh, edge_tuple, dat);
}
auto SplitAlternateFacetData::add_facet(
    const wmtk::Mesh& mesh,
    const wmtk::Tuple& edge_tuple,
    const std::array<int64_t, 2>& nfa) -> const Data&
{
    const PrimitiveType mesh_pt = mesh.top_simplex_type();
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
    return m_facet_maps.emplace_back(sd.dart_from_tuple(edge_tuple), nfa);
}

auto SplitAlternateFacetData::get_alternative_facets(const int64_t& input_cell) const
    -> const std::array<int64_t, 2>&
{
    auto it = get_alternative_facets_it(input_cell);
    assert(it != m_facet_maps.cend());
    return it->new_facet_indices;
}
auto SplitAlternateFacetData::get_alternative(
    const PrimitiveType mesh_pt,
    const Tuple& t,
    const PrimitiveType simplex_dimension) const -> Tuple
{
    return get_alternative(mesh_pt, t);
}
auto SplitAlternateFacetData::get_alternative(const PrimitiveType mesh_pt, const Tuple& t) const
    -> Tuple
{
    assert(mesh_pt > PrimitiveType::Vertex);
    const auto alts_it = get_alternative_facets_it(t.global_cid());
    // assert(alts_it != m_facet_maps.end());
    if (alts_it == m_facet_maps.end()) {
        return {};
    }

    const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);

    int64_t new_global_cid = alts_it->new_gid(mesh_pt, sd.permutation_index_from_tuple(t));


    return {t.local_vid(), t.local_eid(), t.local_fid(), new_global_cid};
}
std::vector<int64_t> SplitAlternateFacetData::get_simplices_to_delete(
    const Mesh& mesh,
    const PrimitiveType& simplex_dim) const
{
    if(simplex_dim < PrimitiveType::Edge) {
        return {};
    }
    const PrimitiveType mesh_pt = mesh.top_simplex_type();
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
    std::vector<int64_t> ret;
    assert(m_facet_maps.size() > 0);
    for (const auto& fm : m_facet_maps) {
        const dart::Dart& d = fm.input;

        for (const int8_t p : wmtk::dart::utils::get_cofaces(
                 mesh_pt,
                 PrimitiveType::Edge,
                 d.permutation(),
                 simplex_dim)) {
            const int64_t facet_id = d.global_id();
            int64_t id = mesh.id(dart::Dart(facet_id, p), simplex_dim);
            ret.emplace_back(id);
        }
    }

    std::sort(ret.begin(), ret.end());
    ret.erase(std::unique(ret.begin(), ret.end()), ret.end());
    return ret;
}
} // namespace wmtk::operations::internal
