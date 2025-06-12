#include "../tuple_map_attribute_io.hpp"
#include "wmtk/Mesh.hpp"
namespace wmtk::multimesh::utils::internal {

void print_all_mapped_tuples(
    const dart::DartAccessor<1, Mesh>& a,
    const dart::DartAccessor<1, Mesh>& b)
{
    const auto& a_mesh = a.mesh();
    const auto& b_mesh = b.mesh();
    auto a_tups = a_mesh.get_all(b_mesh.top_simplex_type());
    auto b_tups = b_mesh.get_all(b_mesh.top_simplex_type());
    const PrimitiveType apt = a.mesh().top_simplex_type();
    const PrimitiveType bpt = b.mesh().top_simplex_type();
    const dart::SimplexDart& asd = dart::SimplexDart::get_singleton(apt);
    const dart::SimplexDart& bsd = dart::SimplexDart::get_singleton(bpt);

    for (const auto& a_t : a_tups) {
        auto a_d_input = asd.dart_from_tuple(a_t);

        dart::Dart b_d = a[a_d_input];
        if (b_d.is_null()) {
            spdlog::info("{}[{}] was unmapped", std::string(a_d_input), a_mesh.id(a_d_input, bpt));
            continue;
        } else {
            dart::Dart a_d = b[b_d];
            //spdlog::info("{} was mapped downward to {}", std::string(a_d), std::string(b_d));
            spdlog::info(
                "{}[{},{}] was mapped downward to {} (removed={})",
                std::string(a_d),
                a_mesh.id(a_d_input, bpt),
                std::string(asd.tuple_from_dart(a_d)),
                std::string(b_d),
                b_mesh.is_removed(b_d.global_id()));
        }
    }
    spdlog::info("parint all giong through {} tups", b_tups.size());
    for (const auto& b_t : b_tups) {
        auto b_d_input = bsd.dart_from_tuple(b_t);

        dart::Dart a_d = b[b_d_input];
        if (a_d.is_null()) {
            spdlog::info("{} was invalid - mapped to nothing", std::string(b_d_input));
            continue;
        } else {
            dart::Dart b_d = a[a_d];
            spdlog::info(
                "{}[{}] was mapped upward to {} (removed={})",
                std::string(b_d),
                std::string(bsd.tuple_from_dart(b_d)),
                std::string(a_d),
                a_mesh.is_removed(a_d.global_id()));
        }
    }
}
} // namespace wmtk::multimesh::utils::internal
