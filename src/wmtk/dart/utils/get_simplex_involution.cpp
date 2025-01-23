#include "get_simplex_involution.hpp"
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
namespace wmtk::dart::utils {

// maps (pt,a) to (opt,oa) then (opt,b)
wmtk::dart::Dart get_simplex_involution(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(pt);

    int8_t oa = sd.convert(a.local_orientation(), osd);

    int8_t act = dart::find_local_dart_action(osd, oa, b.local_orientation());
    return dart::Dart(b.global_id(), act);
}
std::pair<wmtk::dart::Dart, wmtk::dart::Dart> get_simplex_involution_pair(
    PrimitiveType pt,
    const dart::Dart& a,
    PrimitiveType opt,
    const dart::Dart& b)
{
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(pt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(pt);

    int8_t oa = sd.convert(a.local_orientation(), osd);

    int8_t act = dart::find_local_dart_action(osd, oa, b.local_orientation());
    int8_t invact = osd.convert(osd.inverse(act), sd);
    auto ret = std::make_pair(dart::Dart(b.global_id(), act), dart::Dart(a.global_id(), invact));


    return ret;
}
} // namespace wmtk::dart::utils
