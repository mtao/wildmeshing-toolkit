// print debug, TODO: remove
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
//
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
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);

    // if the dimension is not the same, first map to the caonical simplex
    int8_t act;
    if (pt != opt) { // pt < opt
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
        // assert(get_canonical_supdart(sd, opt, a_up) == sd.identity());
        // int8_t bbasis = get_canonical_subdart(sd, opt, b);
        // spdlog::info(
        //    "input target is {}",
        //    fmt::join(dart::utils::get_local_vertex_permutation(pt, b), ","));
        auto [opt_action, bsimplex] = local_simplex_decomposition(sd, opt, b);

        // spdlog::info("{}", fmt::join(dart::utils::get_local_vertex_permutation(pt, bsimplex),
        // ","));

        // spdlog::info(
        //     "local action is {}",
        //     fmt::join(dart::utils::get_local_vertex_permutation(opt, opt_action), ","));

        const int8_t local_lower_act = find_local_dart_left_action(osd, a, opt_action);
        const int8_t local_upper_act = osd.convert(local_lower_act, sd);
        act = sd.product(bsimplex, local_upper_act);


    } else {
        act = dart::find_local_dart_left_action(osd, a, b);
        //spdlog::info(
        //    "global action is {} x {} = {}",
        //    fmt::join(dart::utils::get_local_vertex_permutation(opt, a), ",")
        //    ,fmt::join(dart::utils::get_local_vertex_permutation(opt, act), ",")
        //    ,fmt::join(dart::utils::get_local_vertex_permutation(opt, b), ",")
        //    );
        assert(osd.product(act,a) == b);
        //assert(osd.product(a, act) == b);
    }
    return act;
}


/*
int8_t get_simplex_involution_downwards(
    PrimitiveType pt, // higher dimension
    int8_t a,
    PrimitiveType opt, // lower dimension
    int8_t b)
{
    assert(pt >= opt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);

    // if the dimension is not the same, first map to the caonical simplex
    int8_t act;
    if (pt != opt) {
        const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
        // canonical dart on simplex of type opt (lower type)
        const int8_t abasis = get_canonical_supdart(sd, opt, a);
        // a = act abasis
        act = dart::find_local_dart_action(sd, abasis, a);
        assert(act == sd.product(a,sd.inverse(abasis)));
        //act = sd.product(a,sd.inverse(abasis));

        assert(get_canonical_supdart(sd, opt, act) == sd.identity());


        int8_t b_up = osd.convert(b, sd);
        assert(get_canonical_supdart(sd, opt, b_up) == sd.identity());
        act = sd.product(b_up, sd.inverse(act));

        // act_new = b_up act^{-1}

        int8_t act2 = get_canonical_supdart(sd, opt, act);
        assert(act2 == sd.identity());
        act = sd.product(b_up, abasis);
    } else {
        act = dart::find_local_dart_action(osd, a, b);
    }
    return act;
}
*/
int8_t get_simplex_involution(PrimitiveType pt, const int8_t& a, PrimitiveType opt, const int8_t& b)
{
    /*
    int8_t ret;
    if (pt >= opt) {
        ret = get_simplex_involution_downwards(pt, a, opt, b);
    } else {
        ret = get_simplex_involution_downwards(opt, b, pt, a);
    }
    return ret;
    */

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

    //const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(std::max(pt, opt));
    //int8_t od = osd.inverse(d);
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

    return Dart{a.global_id() ^ b.global_id(), ap};
}

std::pair<wmtk::dart::Dart, wmtk::dart::Dart> get_simplex_involution_pair(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    //auto [ap, bp] = get_simplex_involution_pair(pt, a.permutation(), opt, b.permutation());
    //return std::make_pair(Dart(b.global_id(), ap), Dart(a.global_id(), bp));

    auto d = get_simplex_involution(pt, a, opt, b);
    auto od = get_simplex_involution(opt, b, pt, a);
    return {d,od};
}
} // namespace wmtk::dart::utils
