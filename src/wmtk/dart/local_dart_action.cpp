#include <wmtk/Primitive.hpp>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>

// NOTE: this header primarily exists to simplify unit testing, not really for use
namespace wmtk::dart {
int8_t local_dart_action(const SimplexDart& sd, int8_t s, int8_t action)
{
    return sd.product(action, s);
}
Dart local_dart_action(const SimplexDart& sd, const Dart& d, int8_t action)
{
    return {d.global_id(), local_dart_action(sd, d.permutation(), action)};
}

} // namespace wmtk::dart
