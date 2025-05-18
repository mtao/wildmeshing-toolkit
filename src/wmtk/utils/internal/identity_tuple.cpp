#include "identity_tuple.hpp"
#include <wmtk/dart/SimplexDart.hpp>
namespace wmtk::utils::internal {

Tuple identity_tuple(PrimitiveType pt, int64_t gid)
{
    const auto& sd = dart::SimplexDart::get_singleton(pt);
    return sd.tuple_from_dart(dart::Dart(gid, sd.identity()));
}

} // namespace wmtk::utils::internal
