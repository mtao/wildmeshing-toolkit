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
int8_t SimplexDart::permutation_index_from_primitive_switch(wmtk::PrimitiveType pt) const
{
    //
    return tri_mesh::permutation_index_from_primitive_switch(pt);
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

    return tri_mesh::permutation_indices_dynamic();
}

int8_t SimplexDart::simplex_index(int8_t permutation_index, wmtk::PrimitiveType type) const
{
    if (type < PrimitiveType::Triangle) {
        return tri_mesh::simplex_index(permutation_index, type);
    } else {
        assert(type == PrimitiveType::Triangle);
        return 0;
    }
    //
}
wmtk::PrimitiveType SimplexDart::simplex_type() const
{
    return tri_mesh::constants::primitive_type;
    //
}
} // namespace wmtk::autogen::tri_mesh
