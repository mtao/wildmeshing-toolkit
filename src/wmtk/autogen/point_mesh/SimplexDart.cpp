#include "SimplexDart.hpp"
namespace wmtk::autogen::point_mesh {
namespace constants {
const std::array<int8_t, 1> _indices = {{0}};
}
SimplexDart::~SimplexDart() = default;
int8_t SimplexDart::product(int8_t a, int8_t b) const
{
    return point_mesh::product(a, b);
    //
}
int8_t SimplexDart::inverse(int8_t a) const
{
    return point_mesh::inverse(a);
    //
}
int8_t SimplexDart::permutation_index_from_primitive_switch(wmtk::PrimitiveType pt) const
{
    //
    return point_mesh::permutation_index_from_primitive_switch(pt);
}
int8_t SimplexDart::identity() const
{
    return point_mesh::identity();
    //
}
int8_t SimplexDart::opposite() const
{
    return point_mesh::opposite();
    //
}
size_t SimplexDart::size() const
{
    return point_mesh::size();
    //
}

VectorX<int8_t>::ConstMapType SimplexDart::permutation_indices() const
{
    //

    return point_mesh::permutation_indices_dynamic();
}

int8_t SimplexDart::simplex_index(int8_t permutation_index, wmtk::PrimitiveType type) const
{
    return point_mesh::simplex_index(permutation_index, type);
    //
}
wmtk::PrimitiveType SimplexDart::simplex_type() const
{
    return point_mesh::constants::primitive_type;
    //
}
} // namespace wmtk::autogen::point_mesh
