
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/simplex/cofaces_single_dimension.hpp>
#include "IndexSimplexMapper.hpp"
#include "cofacets.hpp"

namespace wmtk::utils::internal {


namespace detail {
namespace {

template <int8_t SimplexDim, int8_t FaceDim, typename MeshType>
bool is_manifold(const MeshType& m)
{
    spdlog::info(
        "Running is-manifold {} {} [{}]",

        fmt::join(m.absolute_multi_mesh_id(), ","),
        SimplexDim,
        FaceDim);

    PrimitiveType stype = m.top_simplex_type();
    assert(SimplexDim == get_primitive_type_id(stype));
    static_assert(SimplexDim > FaceDim);
    PrimitiveType pt = get_primitive_type_from_id(FaceDim);
    IndexSimplexMapper ism(m);
    auto cofs = cofacets<SimplexDim, FaceDim>(ism);
    std::map<int64_t, std::set<int64_t>> cof_ids;
    for (const auto& [key, values] : cofs) {
        auto& ids = cof_ids[key];
        for (const auto& v : values) {
            ids.emplace(v.global_id());
        }
    }
    for (const auto& t : m.get_all(pt)) {
        auto s = simplex::Simplex(pt, t);
        auto ss = simplex::cofaces_single_dimension(m, s, stype);
        int64_t id = m.id(s);
        std::set<int64_t> cof_ids2;
        for (const simplex::Simplex& cof : ss) {
            cof_ids2.emplace(cof.tuple().global_cid());
        }

        /*
        if ((cof_ids.find(id) == cof_ids.end())) {
            spdlog::info("{}", fmt::join(ism.simplices<SimplexDim>(), ","));
            assert(cof_ids.find(id) == cof_ids.end());
        }
        */

        if (cof_ids2 != cof_ids.at(id)) {
            spdlog::info(
                "{}-simplex {}-face index {} had cofaces from ids: {} and cofaces from topology:{} "
                "as neighbor facets",
                SimplexDim,
                FaceDim,
                id,
                fmt::join(cof_ids.at(id), ","),
                fmt::join(cof_ids2, ","));
            if constexpr (SimplexDim == 1) {
                auto ts = m.get_all(stype);
                for (const auto& facet_id : cof_ids[id]) {
                    auto myt = ts[facet_id];
                    int64_t vid0 = m.id(myt, PrimitiveType::Vertex);
                    int64_t vid1 = m.id(m.switch_vertex(myt), PrimitiveType::Vertex);
                    spdlog::info("{} had vids {} {}", facet_id, vid0, vid1);
                }
                for (const auto& facet_id : cof_ids2) {
                    auto myt = ts[facet_id];
                    int64_t vid0 = m.id(myt, PrimitiveType::Vertex);
                    int64_t vid1 = m.id(m.switch_vertex(myt), PrimitiveType::Vertex);
                    spdlog::info("topo {} had vids {} {}", facet_id, vid0, vid1);
                }
            }
            return false;
        }
    }
    return true;
}

bool is_manifold(const EdgeMesh& m)
{
    return detail::is_manifold<1, 0>(m);
}
bool is_manifold(const TriMesh& m)
{
    return detail::is_manifold<2, 0>(m) && detail::is_manifold<2, 1>(m);
}
bool is_manifold(const TetMesh& m)
{
    return detail::is_manifold<3, 0>(m) && detail::is_manifold<3, 1>(m) &&
           detail::is_manifold<3, 2>(m);
}
} // namespace
} // namespace detail

bool is_manifold(const Mesh& m)
{
    // try {
    if (m.get_all(m.top_simplex_type()).size() == 0) {
        return true;
    }
    switch (m.top_simplex_type()) {
    case wmtk::PrimitiveType::Tetrahedron:
        return detail::is_manifold(*dynamic_cast<const TetMesh*>(&m));
    case wmtk::PrimitiveType::Triangle:
        return detail::is_manifold(*dynamic_cast<const TriMesh*>(&m));
    case wmtk::PrimitiveType::Edge: return detail::is_manifold(*dynamic_cast<const EdgeMesh*>(&m));
    case wmtk::PrimitiveType::Vertex:
    default: break;
    }
    return true;
    //} catch (const std::exception& e) {
    //    throw e;
    //    return false;
    //}
}
} // namespace wmtk::utils::internal

