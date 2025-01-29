
#include "apply_simplex_involution.hpp"
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include "get_canonical_simplex_orientation.hpp"
namespace wmtk::dart::utils {

int8_t apply_simplex_involution_upward(
    int8_t a,
    PrimitiveType pt, // lower dimension
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
    const dart::Dart& action,
    PrimitiveType pt,
    PrimitiveType opt,
    int8_t target_index, // ignored if pt == opt, otherwise it's the index in min(pt,opt) in simplex
                         // (opt,pt)
    const dart::Dart& source)
{
    return dart::Dart(
        action.global_id(),
        apply_simplex_involution_upward(
            action.permutation(),
            pt,
            opt,
            target_index,
            source.permutation()));
}
} // namespace wmtk::dart::utils
