#pragma once
#include <cstdint>
#include <wmtk/dart/Dart.hpp>

namespace wmtk::dart {
class SimplexDart;
}
namespace wmtk::dart::utils {


int8_t opposite_action(int8_t simplex_dimension);
int8_t opposite(const SimplexDart& sd, int8_t permutation_index);
dart::Dart opposite(const SimplexDart& sd, const dart::Dart& dart);

} // namespace wmtk::dart::utils
