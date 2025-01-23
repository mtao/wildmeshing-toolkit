#pragma once
#include <cstdint>

namespace wmtk::dart {
    class SimplexDart;
}
namespace wmtk::dart::utils{


    int8_t edge_mirror(const SimplexDart& sd, int8_t orientation, int8_t edge_dart);
    
}
