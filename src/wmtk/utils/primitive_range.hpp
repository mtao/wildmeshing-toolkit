#pragma once

#include <vector>
#include <wmtk/Primitive.hpp>
namespace wmtk::utils {
PrimitiveType primitive_above(PrimitiveType pt);
PrimitiveType primitive_below(PrimitiveType pt);
// returns a vector of primitives including the endpoints of the range
std::vector<PrimitiveType> primitive_range(PrimitiveType pt0, PrimitiveType pt1);
// returns a vector of primitives including the endpoint
std::vector<PrimitiveType> primitives_above(PrimitiveType pt0, bool lower_to_upper = true);
// returns a vector of primitives including the endpoint
std::vector<PrimitiveType> primitives_below(PrimitiveType pt1, bool lower_to_upper = false);

inline PrimitiveType primitive_above(PrimitiveType pt)
{
    return get_primitive_type_from_id(get_primitive_type_id(pt) + 1);
}
inline PrimitiveType primitive_below(PrimitiveType pt)
{
    return get_primitive_type_from_id(get_primitive_type_id(pt) + 1);
}
} // namespace wmtk::utils
