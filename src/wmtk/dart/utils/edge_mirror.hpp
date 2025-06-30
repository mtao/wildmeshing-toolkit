#pragma once
#include <cstdint>
#include "wmtk/dart/Dart.hpp"

namespace wmtk::dart {
class SimplexDart;

}
namespace wmtk::dart::utils {

int8_t edge_mirror_left_action(const SimplexDart& sd, int8_t edge_permutation);
// with respect to a simplex's local permutation [0,1,2,3]
// this creates a dart such that if [0,1] were collapsed we would have hte same dart
// that is, it swaps 0,1 no matter where they are
int8_t edge_mirror(const SimplexDart& sd, int8_t permutation_index);

dart::Dart edge_mirror(const SimplexDart& sd, const dart::Dart& dart);

int8_t edge_mirror(const SimplexDart& sd, int8_t edge_permutation_index, int8_t dart);

dart::Dart edge_mirror(const SimplexDart& sd, const dart::Dart& edge_dart, const dart::Dart& dart);

} // namespace wmtk::dart::utils
