#include "PointMesh.hpp"

namespace wmtk {
Tuple PointMesh::vertex_tuple_from_id(int64_t id) const
{
    return Tuple(-1, -1, -1, id);
}

PointMesh::~PointMesh() = default;
PointMesh::PointMesh()
    : MeshCRTP<PointMesh>(0)
{}


PointMesh::PointMesh(int64_t size)
    : PointMesh()
{
    initialize(size);
}

Tuple PointMesh::switch_tuple(const Tuple& tuple, PrimitiveType type) const
{
    throw std::runtime_error("Tuple switch: Invalid primitive type");
}
bool PointMesh::is_ccw(const Tuple&) const
{
    // trivial orientation so nothing can happen
    return true;
}
bool PointMesh::is_boundary(PrimitiveType pt, const Tuple& tuple) const
{
    switch (pt) {
    case PrimitiveType::Vertex: return is_boundary_vertex(tuple);
    case PrimitiveType::Edge:
    case PrimitiveType::Triangle:
    case PrimitiveType::Tetrahedron:
    default: break;
    }
    assert(
        false); // "tried to compute the boundary of a point mesh for an invalid simplex dimension"
    // every point is on the interior as it has no boundary simplices
    return false;
}

bool PointMesh::is_boundary_vertex(const Tuple&) const
{
    // every point is on the interior as it has no boundary simplices
    return false;
}

void PointMesh::initialize(int64_t count)
{
    set_capacities({count});
    reserve_attributes_to_fit();
    attribute::FlagAccessor<PointMesh> v_flag_accessor = get_flag_accessor(PrimitiveType::Vertex);
    for (int64_t i = 0; i < capacity(PrimitiveType::Vertex); ++i) {
        v_flag_accessor.index_access().activate(i);
    }
}


bool PointMesh::is_valid(const Tuple& tuple) const
{
    // return !is_removed(tuple);
    if (!Mesh::is_valid(tuple)) {
        return false;
    }
    return true;
}


Tuple PointMesh::tuple_from_id(const PrimitiveType type, const int64_t gid) const
{
    assert(type == PrimitiveType::Vertex); // "Tuple switch: Invalid primitive type"
    return vertex_tuple_from_id(gid);
}

std::vector<std::vector<TypedAttributeHandle<int64_t>>> PointMesh::connectivity_attributes() const
{
    std::vector<std::vector<TypedAttributeHandle<int64_t>>> handles(1);

    return handles;
}

std::vector<Tuple> PointMesh::orient_vertices(const Tuple& tuple) const
{
    return {tuple};
}

} // namespace wmtk
