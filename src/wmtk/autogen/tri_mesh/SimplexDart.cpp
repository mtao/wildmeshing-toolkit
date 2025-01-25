#include "SimplexDart.hpp"
namespace wmtk::autogen::tri_mesh {

SimplexDart::~SimplexDart() = default;
int8_t SimplexDart::product(int8_t a, int8_t b) const
{
    return tri_mesh::product(a, b);
    //
}
int8_t SimplexDart::inverse(int8_t a) const
{
    return tri_mesh::inverse(a);
    //
}
int8_t SimplexDart::primitive_switch_as_permutation_index(wmtk::PrimitiveType pt) const
{
    //
    return tri_mesh::primitive_switch_as_permutation_index(pt);
}
int8_t SimplexDart::identity() const
{
    return tri_mesh::identity();
    //
}
int8_t SimplexDart::opposite() const
{
    return tri_mesh::opposite();
    //
}
size_t SimplexDart::size() const
{
    return tri_mesh::size();
    //
}

VectorX<int8_t>::ConstMapType SimplexDart::permutation_indices() const
{
    //

    return tri_mesh::permutation_indices();
}

int8_t SimplexDart::simplex_index(wmtk::PrimitiveType type, int8_t permutation_index) const
{
    return tri_mesh::simplex_index(type, permutation_index);
    //
}
wmtk::PrimitiveType SimplexDart::simplex_type() const
{
    return tri_mesh::simplex_type();
    //
}
} // namespace wmtk::autogen::tri_mesh