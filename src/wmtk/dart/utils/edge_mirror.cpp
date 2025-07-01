#include "edge_mirror.hpp"
#include <array>
#include <cassert>
#include <exception>
#include <stdexcept>
#include "get_local_vertex_permutation.hpp"
#include "opposite.hpp"
#include "wmtk/dart/SimplexDart.hpp"

#include <fmt/ranges.h>
#include <spdlog/spdlog.h>

namespace wmtk::dart::utils {


/*
namespace {
auto make() -> std::array<int8_t, 4>
{
std::array<int8_t, 4> darts;
darts[0] = 0;
for (int8_t j = 1; j < darts.size(); ++j) {
    PrimitiveType pt = get_primitive_type_from_id(j);
    const wmtk::dart::SimplexDart& sd = wmtk::dart::SimplexDart::get_singleton(pt);
    assert(sd.inverse(sd.opposite()) == sd.opposite()); // the below is a conjugation, but since
                                                        // opp is an involute we can skip that
    darts[j] = sd.product(
        sd.opposite(),
        sd.product(sd.primitive_as_index(wmtk::PrimitiveType::Vertex), sd.opposite()));
}
//
return darts;
}
} // namespace

int8_t edge_mirror_action(int8_t simplex_dimension)
{
const static std::array<int8_t, 4> _data = make();
return _data[simplex_dimension];
}
*/

int8_t edge_mirror_left_action(const SimplexDart& sd, int8_t edge_permutation_index)
{
    const int8_t SV = sd.primitive_as_index(PrimitiveType::Vertex);

    int8_t edge_permutation_index_inv = sd.inverse(edge_permutation_index);
    int8_t tmp = sd.product(edge_permutation_index, SV);
    int8_t r = sd.product(tmp, edge_permutation_index_inv);
    return r;
}
dart::Dart edge_mirror(const SimplexDart& sd, const dart::Dart& dart)
{
    return dart::Dart(dart.global_id(), edge_mirror(sd, dart.permutation()));
}
int8_t edge_mirror(const SimplexDart& sd, int8_t pi)
{
    return sd.product(edge_mirror_left_action(sd, sd.identity()), pi);
}

int8_t edge_mirror(const SimplexDart& sd, int8_t edge_permutation_index, int8_t permutation_index)
{
    int8_t pushforward_action = edge_mirror_left_action(sd, edge_permutation_index);
    const int8_t SV = sd.primitive_as_index(PrimitiveType::Vertex);

    return sd.product(pushforward_action, permutation_index);
}

dart::Dart edge_mirror(const SimplexDart& sd, const dart::Dart& edge_dart, const dart::Dart& dart)
{
    return dart::Dart(
        dart.global_id(),
        edge_mirror(sd, edge_dart.permutation(), dart.permutation()));
}
} // namespace wmtk::dart::utils
