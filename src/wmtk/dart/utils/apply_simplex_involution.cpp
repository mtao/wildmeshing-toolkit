// print debug, TODO: remove
#include <spdlog/spdlog.h>
#include <fmt/ranges.h>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
//

#include "apply_simplex_involution.hpp"
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include "get_canonical_simplex_orientation.hpp"
namespace wmtk::dart::utils {

int8_t apply_simplex_involution_directed(
    PrimitiveType pt, // lower dimension
    PrimitiveType opt, // higher  dimension
    int8_t action, // action, encoded in opt
    int8_t source,
    bool upward)
{
    assert(pt < opt);
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);
    int8_t canonical_orientation = get_canonical_supdart(osd, pt, action);

    int8_t local_action = osd.product(action, osd.inverse(canonical_orientation));


    if (upward) {
        int8_t lifted_source = sd.convert(source, osd);

        spdlog::info("source {} lifted to {}",
                fmt::join(dart::utils::get_local_vertex_permutation(pt, source),","),
                fmt::join(dart::utils::get_local_vertex_permutation(opt, lifted_source),",")
                );

        local_action = osd.inverse(local_action);
        return osd.product(osd.product(local_action, lifted_source), canonical_orientation);
    } else {
        local_action = osd.convert(local_action, sd);
        source = osd.product(source, osd.inverse(canonical_orientation));
        source = osd.convert(source, sd);
        return sd.product(local_action, source);
    }
}


int8_t apply_simplex_involution(
    PrimitiveType pt, // lower dimension
    PrimitiveType opt, // higher  dimension
    int8_t action, // action, encoded in opt
    int8_t source)
{
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);
    int8_t res = osd.product(action, source);

    const bool upward_action = pt < opt;
    if (pt == opt) {
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
        res = sd.product(action, source);
    } else if (pt < opt) {
        return apply_simplex_involution_directed(pt, opt, action, source, true);
    } else { // downward action
        return apply_simplex_involution_directed(opt, pt, action, source, false);
    }
    return res;
}
// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart apply_simplex_involution(
    PrimitiveType pt,
    PrimitiveType opt,
    const dart::Dart& action,
    const dart::Dart& source)
{
    return dart::Dart(
        action.global_id() ^ source.global_id(),
        apply_simplex_involution(pt, opt, action.permutation(), source.permutation()));
}
} // namespace wmtk::dart::utils
