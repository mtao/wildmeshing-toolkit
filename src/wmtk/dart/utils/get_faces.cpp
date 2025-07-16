
#include "get_faces.hpp"
#include "wmtk/dart/SimplexDart.hpp"
#include "wmtk/dart/utils/get_canonical_faces.hpp"
namespace wmtk::dart::utils {

std::vector<int8_t> get_faces(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType face_pt)
{
    if (mesh_pt == target_pt) {
        return {permutation};
    }
    int8_t mesh_dim = get_primitive_type_id(mesh_pt);
    int8_t target_dim = get_primitive_type_id(target_pt);
    int8_t face_dim = get_primitive_type_id(face_pt);

    const auto& mesh_sd = dart::SimplexDart::get_singleton(mesh_pt);
    const auto& target_sd = dart::SimplexDart::get_singleton(target_pt);

    const auto& target_ids = dart::utils::get_canonical_faces(target_sd, face_pt);

    std::vector<int8_t> x(target_ids.begin(), target_ids.end());
    for (int8_t& id : x) {
        id = mesh_sd.product(permutation, target_sd.convert(id, mesh_sd));
    }

    return x;
}
std::vector<int8_t> get_faces_ids(
    const PrimitiveType mesh_pt,
    const PrimitiveType target_pt,
    int8_t permutation,
    const PrimitiveType face_pt)
{
    auto x = get_faces(mesh_pt, target_pt, permutation, face_pt);
    const auto& mesh_sd = dart::SimplexDart::get_singleton(mesh_pt);
    for (int8_t& id : x) {
        id = mesh_sd.simplex_index(id, face_pt);
    }
    return x;
}

} // namespace wmtk::dart::utils
