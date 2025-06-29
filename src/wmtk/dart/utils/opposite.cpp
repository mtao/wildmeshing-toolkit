
#include "opposite.hpp"
#include <array>
#include <cassert>
#include "wmtk/dart/SimplexDart.hpp"


namespace wmtk::dart::utils {

namespace {
auto make_opp_actions() -> std::array<int8_t, 4>
{
    std::array<int8_t, 4> darts;
    darts[0] = 0;
    for (int8_t j = 1; j < darts.size(); ++j) {
        PrimitiveType pt = get_primitive_type_from_id(j);
        const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(pt);
        darts[j] = sd.opposite();
    }
    //
    return darts;
}
} // namespace

int8_t opposite_action(int8_t simplex_dimension)
{
    const static std::array<int8_t, 4> _data = make_opp_actions();
    return _data[simplex_dimension];
}

int8_t opposite(const SimplexDart& sd, int8_t permutation_index)
{
    return sd.act(permutation_index, opposite_action(get_primitive_type_id(sd.simplex_type())));
}
dart::Dart opposite(const SimplexDart& sd, const dart::Dart& dart)
{
    return dart::Dart(dart.global_id(), opposite(sd, dart.permutation()));
}

} // namespace wmtk::dart::utils
