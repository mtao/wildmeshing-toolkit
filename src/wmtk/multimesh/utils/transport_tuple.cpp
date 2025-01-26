#include "transport_tuple.hpp"
//#include <wmtk/utils/TupleInspector.hpp>
//#include <spdlog/spdlog.h>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>
#include "find_local_dart_action.hpp"
#include "find_local_switch_sequence.hpp"
#include "local_switch_tuple.hpp"

namespace wmtk::multimesh::utils {

namespace internal {
Tuple transport_tuple_sequence(
    const Tuple& base_source,
    const Tuple& base_target,
    PrimitiveType base_primitive_type,
    const Tuple& source,
    PrimitiveType primitive_type)
{
    std::vector<PrimitiveType> operations =
        find_local_switch_sequence(base_source, base_target, base_primitive_type);

    return local_switch_tuples(primitive_type, source, operations);
}
Tuple transport_tuple_dart(
    const Tuple& base_source,
    const Tuple& base_target,
    PrimitiveType base_primitive_type,
    const Tuple& source,
    PrimitiveType primitive_type)
{
    const auto& base_sd = wmtk::dart::SimplexDart::get_singleton(base_primitive_type);
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(primitive_type);
    return transport_tuple(base_sd, sd, base_source, base_target, source);
}
} // namespace internal
Tuple transport_tuple(
    const Tuple& base_source,
    const Tuple& base_target,
    PrimitiveType base_primitive_type,
    const Tuple& source,
    PrimitiveType primitive_type)
{
    assert(!base_source.is_null());
    assert(!base_target.is_null());
    assert(!source.is_null());
    auto dart = internal::transport_tuple_dart(
        base_source,
        base_target,
        base_primitive_type,
        source,
        primitive_type);


    /*
    auto seq = internal::transport_tuple_sequence(
        base_source,
        base_target,
        base_primitive_type,
        source,
        primitive_type);
        return seq;
    assert( dart == seq);
    */
    assert(!dart.is_null());
    return dart;
}

Tuple transport_tuple(
    const wmtk::dart::SimplexDart& base_sd,
    const wmtk::dart::SimplexDart& sd,
    const Tuple& base_source,
    const Tuple& base_target,
    const Tuple& source)
{
    const int8_t base_action = find_local_dart_action(base_sd, base_source, base_target);
    const int8_t action = base_sd.convert(base_action, sd);


    int8_t src_dart = sd.permutation_index_from_tuple(source);
    const int8_t tgt_dart = sd.product(action, src_dart);
    //spdlog::warn("base action{} final action{} src_dart{} tgt_dart{}", base_action, action, src_dart, tgt_dart);
    const auto r = sd.update_tuple_from_permutation_index(source, tgt_dart);
    //spdlog::info("{}", wmtk::utils::TupleInspector::as_string(r));
    return r;
}
} // namespace wmtk::multimesh::utils
