#pragma once
#include <cstdint>
#include <wmtk/PrimitiveType.hpp>

namespace wmtk::dart{
class SimplexDart;
}
namespace wmtk::dart::utils {
/// @brief: Returns the relative local index of a simplex with respect to an permutation
/// When we switch a dart to lie on a specific simplex we need to find some action that maps to that simplex.
/// We typically encode these target simplices in terms of indices used in the identity permutation, so this function rewrites an index with repsect to the identity permutation to the given input permutation
/// @param sd the context for the type of simplex we are in
/// @param current_permutation the permutation we want to get the relative ilocal index of
/// @param target_type the type of simplex we are trying to get the relative index of
/// @param the global index of a simplex with respect to the identity dart
int8_t relative_face_index(
    const SimplexDart& sd,
    int8_t current_permutation,
    PrimitiveType target_type,
    int8_t target_index);

} // namespace wmtk::autogen::utils
