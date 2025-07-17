

#include "get_cofaces.hpp"
#include "get_faces.hpp"
#include "get_local_vertex_permutation.hpp"
#include "wmtk/dart/SimplexDart.hpp"
#include "wmtk/dart/utils/get_canonical_faces.hpp"
namespace wmtk::dart::utils {

std::vector<int8_t> get_cofaces(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType coface_pt)
{
    if (coface_pt == target_pt) {
        return {permutation};
    }
    int8_t mesh_dim =
        get_primitive_type_id(mesh_pt); // 3-simplex <-- example numbers to verify off-by-oine
    int8_t target_dim = get_primitive_type_id(target_pt); // 1-simplex
    int8_t coface_dim = get_primitive_type_id(coface_pt); // 2-simplex
    int8_t dual_face_dim = mesh_dim - coface_dim; // (3-2) == 1
    const PrimitiveType dual_face_pt = get_primitive_type_from_id(dual_face_dim);
    int8_t dual_target_dim = mesh_dim - target_dim; // (3-1) == 2
    const PrimitiveType dual_target_pt = get_primitive_type_from_id(dual_target_dim);


    const auto& mesh_sd = dart::SimplexDart::get_singleton(mesh_pt);
    const auto& target_sd = dart::SimplexDart::get_singleton(target_pt);
    const auto& dual_target_sd = dart::SimplexDart::get_singleton(dual_target_pt);

    const auto& dual_target_ids = dart::utils::get_canonical_faces(dual_target_sd, dual_face_pt);


    const int8_t opp = mesh_sd.opposite();
    std::vector<int8_t> x(dual_target_ids.begin(), dual_target_ids.end());
    for (int8_t& id : x) {
        // id = mesh_sd.product(permutation, target_sd.convert(id, mesh_sd));
        int8_t c = target_sd.convert(id, mesh_sd);


        id = mesh_sd.product({permutation, opp, c, opp});
    }

    return x;
}

std::vector<int8_t> get_cofaces_ids(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType coface_pt)
{
    auto x = get_cofaces(mesh_pt, target_pt, permutation, coface_pt);
    const auto& mesh_sd = dart::SimplexDart::get_singleton(mesh_pt);
    for (int8_t& id : x) {
        id = mesh_sd.simplex_index(id, coface_pt);
    }
    return x;
}

} // namespace wmtk::dart::utils
