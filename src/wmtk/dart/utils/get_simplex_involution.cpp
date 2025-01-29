#include "get_simplex_involution.hpp"
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include "get_canonical_simplex_orientation.hpp"
namespace wmtk::dart::utils {

// maps (pt,a) to (opt,oa) then (opt,b)
int8_t get_simplex_involution_downwards(
    PrimitiveType pt, // higher dimension
    int8_t a,
    PrimitiveType opt, // lower dimension
    int8_t b)
{
    assert(pt >= opt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);

    if (pt > opt) {
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
        int8_t abasis = get_canonical_simplex_orientation(sd, opt, sd.simplex_index(a, opt));
        a = dart::find_local_dart_action(sd, abasis, a);
        a = sd.convert(a, osd);
    } else {
    }
    int8_t act = dart::find_local_dart_action(osd, a, b);
    return act;
}
int8_t get_simplex_involution(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b)
{
    int8_t ret;
    ret = get_simplex_involution_downwards(pt, a, opt, b);
    if (pt < opt) {
        const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);
        ret = osd.inverse(ret);
    }
    return ret;
}
std::pair<int8_t, int8_t>
get_simplex_involution_pair(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b)
{
    // opt is greater, so the dart is encoded in terms of opt
    auto d = get_simplex_involution(pt, a, opt, b);

    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(std::max(pt, opt));
    int8_t od = osd.inverse(d);
    return {d, od};
}

// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart get_simplex_involution(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    auto ap = get_simplex_involution(pt, a.permutation(), opt, b.permutation());

    return Dart{b.global_id(), ap};
}

std::pair<wmtk::dart::Dart, wmtk::dart::Dart> get_simplex_involution_pair(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    auto [ap, bp] = get_simplex_involution_pair(pt, a.permutation(), opt, b.permutation());

    return std::make_pair(Dart(b.global_id(), ap), Dart(a.global_id(), bp));
}
} // namespace wmtk::dart::utils
