#pragma once
#include <span>
#include <wmtk/dart/SimplexDart.hpp>

namespace wmtk::dart {
    class SimplexDart;
}
namespace wmtk::dart::utils {

    std::span<const int8_t> get_canonical_faces(const SimplexDart& sd, PrimitiveType pt);

} // namespace wmtk::dart::utils
