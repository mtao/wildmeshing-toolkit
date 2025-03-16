#include "Involution.hpp"
#include "utils/apply_simplex_involution.hpp"
#include "utils/get_simplex_involution.hpp"
namespace wmtk::dart {
Involution::Involution(PrimitiveType spt, const Dart& s, PrimitiveType tpt, const Dart& t)
    : source(spt)
    , target(tpt)
    , involution(utils::get_simplex_involution(spt, s, tpt, t))
{}

Dart Involution::apply(const Dart& d) const
{
    return utils::apply_simplex_involution(involution, source, target, d);
}

Dart Involution::invert(const Dart& d) const
{
    if (source == target) {
    } else {
        return utils::apply_simplex_involution(involution, source, target, d);
    }
}

} // namespace wmtk::dart
