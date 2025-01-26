#pragma once
#include <vector>
#include <wmtk/Primitive.hpp>
#include <wmtk/Tuple.hpp>
namespace wmtk::dart {
class SimplexDart;
class Dart;
} // namespace wmtk::dart

namespace wmtk::multimesh::utils {

// Maps the tuple source according to the operation sequence
// std::vector<PrimitiveType> operations where operations satisfies
// base_target = switch_tuples(base_source, operations)
int8_t find_local_dart_action(PrimitiveType mesh_type, const Tuple& source, const Tuple& target);
int8_t
find_local_dart_action(const wmtk::dart::SimplexDart& sd, const Tuple& source, const Tuple& target);
int8_t find_local_dart_action(
    const wmtk::dart::SimplexDart& sd,
    const wmtk::dart::Dart& source,
    const wmtk::dart::Dart& target);
int8_t find_local_dart_action(
    PrimitiveType pt,
    const wmtk::dart::Dart& source,
    const wmtk::dart::Dart& target);
} // namespace wmtk::multimesh::utils
