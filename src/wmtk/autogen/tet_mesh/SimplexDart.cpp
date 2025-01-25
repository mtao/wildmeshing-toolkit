#include "SimplexDart.hpp"
namespace wmtk::autogen::tet_mesh {

SimplexDart::~SimplexDart() = default;
int8_t SimplexDart::product(int8_t a, int8_t b) const
{
    return tet_mesh::product(a, b);
    //
}
int8_t SimplexDart::inverse(int8_t a) const
{
    return tet_mesh::inverse(a);
    //
}
int8_t SimplexDart::primitive_switch_as_permutation_index(wmtk::PrimitiveType pt) const
{
    //
    return tet_mesh::primitive_switch_as_permutation_index(pt);
}
int8_t SimplexDart::identity() const
{
    return tet_mesh::identity();
    //
}
int8_t SimplexDart::opposite() const
{
    return tet_mesh::opposite();
    //
}
size_t SimplexDart::size() const
{
    return tet_mesh::size();
    //
}

VectorX<int8_t>::ConstMapType SimplexDart::permutation_indices() const
{
    //

    return tet_mesh::permutation_indices();
}

int8_t SimplexDart::simplex_index(wmtk::PrimitiveType type, int8_t permutation_index) const
{
    return tet_mesh::simplex_index(type, permutation_index);
    //
}
wmtk::PrimitiveType SimplexDart::simplex_type() const
{
    return tet_mesh::simplex_type();
    //
}
} // namespace wmtk::autogen::tet_mesh