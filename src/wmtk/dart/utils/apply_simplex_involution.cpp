// print debug, TODO: remove
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
//

#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include "apply_simplex_involution.hpp"
#include "get_canonical_simplex_orientation.hpp"
#include "local_simplex_decomposition.hpp"
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

    auto [local_action, target_simplex] = local_simplex_decomposition(osd, pt, action);


    if (upward) {
        int8_t target_orientation = sd.product(local_action, source);
        //int8_t target_orientation = sd.product(source, local_action);
        int8_t lifted_source = sd.convert(target_orientation, osd);

        return osd.product(target_simplex, lifted_source);
    } else {
        auto [local_perm, _] = local_simplex_decomposition(osd, pt, source);

        //spdlog::info(
        //    "global action  in apply is {} x {} = {}",
        //    fmt::join(dart::utils::get_local_vertex_permutation(pt, local_perm), ",")
        //    ,fmt::join(dart::utils::get_local_vertex_permutation(pt, local_action), ",")
        //    ,fmt::join(dart::utils::get_local_vertex_permutation(pt, sd.product( local_action, local_perm)), ",")
        //    );
        return sd.product(sd.inverse(local_action), local_perm);
        //return sd.product(local_perm, local_action);
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
        res = sd.product( action, source);
        //res = sd.product(source, action);
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
        action.global_id() ,
        //action.global_id() ^ source.global_id(),
        apply_simplex_involution(pt, opt, action.permutation(), source.permutation()));
}
} // namespace wmtk::dart::utils
