


#include "local_simplex_decomposition.hpp"
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include "../find_local_dart_action.hpp"
#include "get_canonical_subdart.hpp"
#include "get_canonical_supdart.hpp"

namespace wmtk::dart::utils {

// Given a sd_pt-dart, returns a pt-dart "r" and base_pt-dart "o", where "r" is the relative
// orientation compared to the canonical "o"-simplex
std::pair<int8_t, int8_t>
local_simplex_decomposition(const SimplexDart& sd, PrimitiveType pt, int8_t permutation)
{
    const PrimitiveType source_pt = sd.simplex_type();
    if (source_pt != pt) {
        const int8_t o = get_canonical_supdart(sd, pt, permutation);

        const SimplexDart& subsd = SimplexDart::get_singleton(pt);

        const int8_t upper_perm = find_local_dart_action(sd, o, permutation);

        const int8_t lower_perm = sd.convert(upper_perm, subsd);

        spdlog::info(
            "global action is {}:{} is {}:{}",
            upper_perm,
            fmt::join(dart::utils::get_local_vertex_permutation(source_pt, upper_perm), ","),
            lower_perm,
            fmt::join(dart::utils::get_local_vertex_permutation(pt, lower_perm), ","));

        return std::make_pair(lower_perm, o);
    } else {
        return std::make_pair(permutation, sd.identity());
    }
}
} // namespace wmtk::dart::utils
