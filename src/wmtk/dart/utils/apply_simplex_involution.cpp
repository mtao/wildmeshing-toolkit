
#include "apply_simplex_involution.hpp"
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include "get_canonical_simplex_orientation.hpp"
namespace wmtk::dart::utils {

int8_t apply_simplex_involution_upward(
    PrimitiveType pt, // lower dimension
    int8_t a,
    PrimitiveType opt, // higher  dimension
    int8_t local_index,
    int8_t b)
{
    assert(pt <= opt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);
    int8_t res = osd.product(a, b);
    if (pt < opt) {
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
        int8_t abasis = get_canonical_simplex_orientation(sd, opt, local_index);
        res = sd.product(res, abasis);
    }
    return res;
}
// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart apply_simplex_involution(
    PrimitiveType pt,
    const dart::Dart& action,
    PrimitiveType opt,
    int8_t target_index, // ignored if pt == opt, otherwise it's the index in min(pt,opt) in simplex
                         // (opt,pt)
    const dart::Dart& source)
{
    return dart::Dart(
        action.global_id(),
        apply_simplex_involution_upward(
            pt,
            action.permutation(),
            opt,
            target_index,
            source.permutation()));
}

int8_t apply_simplex_involution_upward(
    PrimitiveType pt, // lower dimension
    PrimitiveType opt, // higher  dimension
    int8_t action,
    int8_t source)
{
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);
    int8_t res = osd.product(action, source);

    const bool upward_action = pt < opt;
    const PrimitiveType action_pt = upward_action ? opt : pt;
    const PrimitiveType lower_pt = upward_action ? pt : opt;
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(action_pt);
    if (pt == opt) {
        res = sd.product(action, source);
    } else if (upward_action) {
        int8_t canonical_orientation = get_canonical_supdart(sd, lower_pt, action);
        int8_t local_action = sd.product(action, sd.inverse(canonical_orientation));

        res = sd.product(res, action);
    } else { // downward action
        int8_t canonical_orientation = get_canonical_supdart(sd, lower_pt, action);
        int8_t local_action = sd.product(action, sd.inverse(canonical_orientation));
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
        apply_simplex_involution_upward(pt, opt, action.permutation(), source.permutation()));
}
} // namespace wmtk::dart::utils
