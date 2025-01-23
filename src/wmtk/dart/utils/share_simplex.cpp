#include "share_simplex.hpp"
#include <wmtk/dart/SimplexDart.hpp>


namespace wmtk::dart::utils {
    bool share_simplex(PrimitiveType mesh_type, PrimitiveType simplex_type, int8_t a, int8_t b) {
    const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(mesh_type);

    return sd.simplex_index(a, simplex_type) == sd.simplex_index(b,simplex_type);

    }
}
