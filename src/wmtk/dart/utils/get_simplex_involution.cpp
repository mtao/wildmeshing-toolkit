#include "get_simplex_involution.hpp"
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
namespace wmtk::dart::utils {

// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart get_simplex_involution_downwards(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    assert(pt >= opt);
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);

    int8_t oa = sd.convert(a.local_orientation(), osd);

    int8_t act = dart::find_local_dart_action(osd, oa, b.local_orientation());
    return dart::Dart(b.global_id(), act);
}
wmtk::dart::Dart get_simplex_involution(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    if(pt >= opt) {
        return get_simplex_involution_downwards(pt,a,opt,b);
    } else {
        // opt is greater, so the dart is encoded in terms of opt
        auto dart = get_simplex_involution_downwards(pt,a,opt,b);
        const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(opt);
        dart.local_orientation() = osd.inverse(dart.local_orientation());
        return dart;
    }
}
std::pair<wmtk::dart::Dart, wmtk::dart::Dart> get_simplex_involution_pair(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
        // opt is greater, so the dart is encoded in terms of opt
        auto d = get_simplex_involution(pt,a,opt,b);

        const dart::SimplexDart& osd = dart::SimplexDart::get_singleton( std::max(pt,opt));
        wmtk::dart::Dart od(a.global_id(), osd.inverse(d.local_orientation()));
        return {d,od};
    }
} // namespace wmtk::dart::utils
