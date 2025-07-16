#pragma once
#include <span>
#include <wmtk/dart/SimplexDart.hpp>

namespace wmtk::dart {
    class SimplexDart;
}
namespace wmtk::dart::utils {

    //std::span<const int8_t> get_canonical_cofaces(const SimplexDart& sd, PrimitiveType pt);
    // NOTE: this is slow, best to cache these values
    std::vector<std::vector<int8_t>> get_canonical_cofaces(const SimplexDart& sd, PrimitiveType pt, PrimitiveType coface_pt);

} // namespace wmtk::dart::utils
