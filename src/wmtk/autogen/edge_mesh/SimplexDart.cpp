#include "SimplexDart.hpp"
namespace wmtk::autogen::edge_mesh {

SimplexDart::~SimplexDart() = default;
int8_t SimplexDart::product(int8_t a, int8_t b) const
{
    return edge_mesh::product(a, b);
    //
}
int8_t SimplexDart::inverse(int8_t a) const
{
    return edge_mesh::inverse(a);
    //
}
int8_t SimplexDart::permutation_index_from_primitive_switch(wmtk::PrimitiveType pt) const
{
    //
    return edge_mesh::permutation_index_from_primitive_switch(pt);
}
int8_t SimplexDart::identity() const
{
    return edge_mesh::identity();
    //
}
int8_t SimplexDart::opposite() const
{
    return edge_mesh::opposite();
    //
}
size_t SimplexDart::size() const
{
    return edge_mesh::size();
    //
}

VectorX<int8_t>::ConstMapType SimplexDart::permutation_indices() const
{
    //

    return edge_mesh::permutation_indices_dynamic();
}

int8_t SimplexDart::simplex_index(int8_t permutation_index, wmtk::PrimitiveType type) const
{
    if (type < constants::primitive_type) {
        return edge_mesh::simplex_index(permutation_index, type);
    } else {
        assert(type == constants::primitive_type);
        return 0;
    }
    //
}
wmtk::PrimitiveType SimplexDart::simplex_type() const
{
    return edge_mesh::constants::primitive_type;
    //
}
} // namespace wmtk::autogen::edge_mesh