#include "SimplexDart.hpp"
namespace wmtk::autogen::tet_mesh {

SimplexDart::~SimplexDart() {}
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
int8_t SimplexDart::permutation_index_from_primitive_switch(wmtk::PrimitiveType pt) const
{
    //
    return tet_mesh::permutation_index_from_primitive_switch(pt);
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

    return tet_mesh::permutation_indices_dynamic();
}

int8_t SimplexDart::simplex_index(int8_t permutation_index, wmtk::PrimitiveType type) const
{
    return tet_mesh::simplex_index(permutation_index, type);
    //
}
wmtk::PrimitiveType SimplexDart::simplex_type() const
{
    return tet_mesh::constants::primitive_type;
    //
}
} // namespace wmtk::autogen::tet_mesh