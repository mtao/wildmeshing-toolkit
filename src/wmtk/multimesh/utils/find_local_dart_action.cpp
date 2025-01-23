
#include "find_local_dart_action.hpp"

#include <cassert>
#include <optional>
#include <stdexcept>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/find_local_dart_action.hpp>
#include <wmtk/utils/TupleInspector.hpp>
#include "local_switch_tuple.hpp"
namespace wmtk::multimesh::utils {
int8_t find_local_dart_action(PrimitiveType pt, const Tuple& source, const Tuple& target)
{
    wmtk::dart::SimplexDart sd(pt);
    return find_local_dart_action(sd, source, target);
}
int8_t find_local_dart_action(
    const wmtk::dart::SimplexDart& sd,
    const Tuple& source,
    const Tuple& target)
{
    // target = R * source
    // target * source^{-1} = R
    int8_t src = sd.valid_index_from_tuple(source);
    int8_t tgt = sd.valid_index_from_tuple(target);
    return wmtk::dart::find_local_dart_action(sd, src, tgt);
}
int8_t find_local_dart_action(
    const wmtk::dart::SimplexDart& sd,
    const wmtk::dart::Dart& source,
    const wmtk::dart::Dart& target)
{
    return wmtk::dart::find_local_dart_action(
        sd,
        source.local_orientation(),
        target.local_orientation());
}

} // namespace wmtk::multimesh::utils
