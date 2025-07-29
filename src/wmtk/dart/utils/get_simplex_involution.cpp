#include <wmtk/autogen/subgroup/convert.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include "get_canonical_subdart.hpp"
#include "get_simplex_involution.hpp"
#include "local_simplex_decomposition.hpp"
namespace wmtk::dart::utils {

// let a be a k-dart and b be a j-dart (k >= j)
// We want an operator such that for any $k$-dart that shares $j$-simplex with $a$ wehave a^{-1}
// LEt C be teh canonical supdart of b. Then let
// d = b C^{-1}  <- the left action from the canonical supdart to b
// encode a^{-1}bC^{-1}
//
//

int8_t get_simplex_involution_upward(
    PrimitiveType opt, // lower dimension
    int8_t a,
    PrimitiveType pt, // higher dimension
    int8_t b)
{
    assert(pt >= opt);
    if (opt == PrimitiveType::Vertex) {
        return b;
    }
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);

    // if the dimension is not the same, first map to the caonical simplex
    int8_t act;
    if (pt != opt) { // pt < opt
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
        // assert(get_canonical_supdart(sd, opt, a_up) == sd.identity());
        // int8_t bbasis = get_canonical_subdart(sd, opt, b);
        auto [opt_action, bsimplex] = local_simplex_decomposition(sd, opt, b);


        const int8_t local_lower_act = find_local_dart_left_action(osd, a, opt_action);
        assert(wmtk::autogen::subgroup::can_convert(opt, pt, local_lower_act));
        const int8_t local_upper_act = osd.convert(local_lower_act, sd);
        act = sd.product(bsimplex, local_upper_act);


    } else {
        act = dart::find_local_dart_left_action(osd, a, b);
        assert(osd.product(act, a) == b);
        // assert(osd.product(a, act) == b);
    }
    return act;
}


int8_t get_simplex_involution(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b)
{
    int8_t ret;
    if (pt <= opt) {
        ret = get_simplex_involution_upward(pt, a, opt, b);
    } else {
        ret = get_simplex_involution_upward(opt, b, pt, a);
    }
    return ret;
}
std::pair<int8_t, int8_t>
get_simplex_involution_pair(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b)
{
    // opt is greater, so the dart is encoded in terms of opt
    auto d = get_simplex_involution(pt, a, opt, b);
    auto od = get_simplex_involution(opt, b, pt, a);

    // const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(std::max(pt, opt));
    // int8_t od = osd.inverse(d);
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
    // return Dart{a.global_id() ^ b.global_id(), ap};
}

std::pair<wmtk::dart::Dart, wmtk::dart::Dart> get_simplex_involution_pair(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    // auto [ap, bp] = get_simplex_involution_pair(pt, a.permutation(), opt, b.permutation());
    // return std::make_pair(Dart(b.global_id(), ap), Dart(a.global_id(), bp));

    auto d = get_simplex_involution(pt, a, opt, b);
    auto od = get_simplex_involution(opt, b, pt, a);
    return {d, od};
}
} // namespace wmtk::dart::utils
