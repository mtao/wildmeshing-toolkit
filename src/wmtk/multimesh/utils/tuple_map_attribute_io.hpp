#pragma once
#include <wmtk/Types.hpp>
#include <wmtk/attribute/Accessor.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_simplex_involution.hpp>
#include <wmtk/multimesh/MultiMeshManager.hpp>

// #define WMTK_DISABLE_COMPRESSED_MULTIMESH_TUPLE
namespace wmtk::multimesh::utils {


template <typename MeshA, typename MeshB>
void symmetric_write_tuple_map_attributes(
    wmtk::dart::DartAccessor<1, MeshA>& a_to_b,
    wmtk::dart::DartAccessor<1, MeshB>& b_to_a,
    const dart::Dart& a,
    const dart::Dart& b)
{
#if !defined(NDEBUG)
    const PrimitiveType apt = a_to_b.mesh().top_simplex_type();
    const PrimitiveType bpt = b_to_a.mesh().top_simplex_type();
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(apt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(bpt);
    assert(sd.is_valid(a));
    assert(osd.is_valid(b));
    PrimitiveType pt = std::min(apt, bpt);
    spdlog::warn(
        "UPdating {}-mesh to {}-mesh darts {} => {} (gids were {} => {})",
        apt,
        bpt,
        a,
        b,
        a_to_b.mesh().id(a, pt),
        b_to_a.mesh().id(b, pt));
#endif
    std::tie(a_to_b[a], b_to_a[b]) = wmtk::dart::utils::get_simplex_involution_pair(apt, a, bpt, b);
}

template <typename MeshA, typename MeshB>
void symmetric_write_tuple_map_attributes(
    wmtk::dart::DartAccessor<1, MeshA>& a_to_b,
    wmtk::dart::DartAccessor<1, MeshB>& b_to_a,
    const Tuple& a_tuple,
    const Tuple& b_tuple)
{
    const PrimitiveType apt = a_to_b.mesh().top_simplex_type();
    const PrimitiveType bpt = b_to_a.mesh().top_simplex_type();
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(apt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(bpt);
    auto a = sd.dart_from_tuple(a_tuple);
    auto b = osd.dart_from_tuple(b_tuple);
    return symmetric_write_tuple_map_attributes<MeshA, MeshB>(a_to_b, b_to_a, a, b);
}
} // namespace wmtk::multimesh::utils
