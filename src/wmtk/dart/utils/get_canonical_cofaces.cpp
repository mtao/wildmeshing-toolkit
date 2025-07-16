#include <wmtk/dart/SimplexDart.hpp>
#include "get_canonical_faces.hpp"
#include "get_faces.hpp"

namespace wmtk::dart::utils {
std::vector<std::vector<int8_t>>
get_canonical_cofaces(const SimplexDart& sd, PrimitiveType pt, PrimitiveType coface_pt)
{
    size_t face_count = get_canonical_faces(sd, pt).size();
    size_t simplex_count = get_canonical_faces(sd, coface_pt).size();

    std::vector<std::vector<int8_t>> r(face_count);

    for (int8_t simplex = 0; simplex < simplex_count; ++simplex) {
        auto faces = get_faces(sd.simplex_type(), coface_pt, simplex, pt);
        for (int8_t face : faces) {
            r[face].emplace_back(simplex);
        }
    }

    return r;
}
} // namespace wmtk::dart::utils
