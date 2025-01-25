#pragma once
#include <cstdint>

namespace wmtk::dart {
    class SimplexDart;
}
namespace wmtk::dart::utils{


    int8_t edge_mirror(const SimplexDart& sd, int8_t permutation_index, int8_t edge_dart);
    
}
