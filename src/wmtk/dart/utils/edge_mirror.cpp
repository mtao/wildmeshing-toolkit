#include "edge_mirror.hpp"
#include <array>
#include <cassert>
#include <exception>
#include <stdexcept>
#include "opposite.hpp"
#include "wmtk/dart/SimplexDart.hpp"
#include "get_local_vertex_permutation.hpp"

#include <spdlog/spdlog.h>
#include <fmt/ranges.h>

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

int8_t edge_mirror(const SimplexDart& sd, int8_t permutation_index)
{

    return sd.product(sd.primitive_as_index(PrimitiveType::Vertex),permutation_index);
    //return sd.product(permutation_index,sd.product(sd.primitive_as_index(PrimitiveType::Vertex),sd.inverse(permutation_index)));

    //return sd.act(permutation_index, edge_mirror_action(get_primitive_type_id(sd.simplex_type())));
}
dart::Dart edge_mirror(const SimplexDart& sd, const dart::Dart& dart)
{
    return dart::Dart(dart.global_id(), edge_mirror(sd, dart.permutation()));
}

int8_t edge_mirror(const SimplexDart& sd, int8_t edge_permutation_index, int8_t permutation_index) {

    const int8_t SV = sd.primitive_as_index(PrimitiveType::Vertex);
    int8_t pushforward_action = sd.product(sd.inverse(edge_permutation_index),sd.product(SV,edge_permutation_index));
    spdlog::info("primitive swap {} got mapped to {} from edge {}",
            fmt::join(get_local_vertex_permutation(sd.simplex_type(),SV),","),
            fmt::join(get_local_vertex_permutation(sd.simplex_type(),pushforward_action),","),
            fmt::join(get_local_vertex_permutation(sd.simplex_type(),edge_permutation_index),","));

    spdlog::info("{}", fmt::join(get_local_vertex_permutation(sd.simplex_type(),//
    sd.product(SV,edge_permutation_index)
    //
                        ),","));
    spdlog::info("{}", fmt::join(get_local_vertex_permutation(sd.simplex_type(),//
    sd.inverse(edge_permutation_index)
    //
                        ),","));
    spdlog::info("{}", fmt::join(get_local_vertex_permutation(sd.simplex_type(),//
    sd.product(sd.inverse(edge_permutation_index),
    sd.product(SV,edge_permutation_index))
    //
                        ),","));

    return sd.product(pushforward_action, permutation_index);
}

dart::Dart edge_mirror(const SimplexDart& sd, const dart::Dart& edge_dart, const dart::Dart& dart) {
    return dart::Dart(dart.global_id(), edge_mirror(sd, edge_dart.permutation(), dart.permutation()));
}
} // namespace wmtk::dart::utils
