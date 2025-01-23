#pragma once
#include <wmtk/autogen/subgroup/convert.hpp>
#include <wmtk/PrimitiveType.hpp>

namespace wmtk::dart::subgroup {

    // converts a dart, as a permutation action, from one primitive type to another
    // can return -1 if no mapping exists
    inline int8_t convert(PrimitiveType from, PrimitiveType to, int8_t source) {
        return autogen::subgroup::convert(from,to,source);
    }
    inline int8_t convert(int8_t from, int8_t to, int8_t source) {
        return autogen::subgroup::convert(from,to,source);
    }

    inline bool can_convert(PrimitiveType from, PrimitiveType to, int8_t source) {
        return autogen::subgroup::can_convert(from,to,source);
    }


}

