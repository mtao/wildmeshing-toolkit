
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/simplex/faces_single_dimension.hpp>
#include <wmtk/simplex/link.hpp>
#include "valence.hpp"
namespace wmtk::simplex {
int64_t valence(const Mesh& mesh, const Tuple& vertex)
{
    switch (mesh.top_simplex_type()) {
    case PrimitiveType::Triangle: return valence(static_cast<const TriMesh&>(mesh), vertex);
    case PrimitiveType::Tetrahedron: return valence(static_cast<const TetMesh&>(mesh), vertex);
    case PrimitiveType::Vertex:
    case PrimitiveType::Edge:
    default:
        return static_cast<int64_t>(link(mesh, simplex::Simplex::vertex(vertex))
                                        .simplex_vector(PrimitiveType::Vertex)
                                        .size());
    }
}
int64_t valence(const TriMesh& mesh, const Tuple& vertex)
{
    const simplex::Simplex v0 = simplex::Simplex::vertex(mesh, vertex);

    return static_cast<int64_t>(
        link(mesh, v0).simplex_vector(PrimitiveType::Vertex).size());
}

int64_t valence(const TetMesh& mesh, const Tuple& vertex)
{
    return static_cast<int64_t>(
        link(mesh, simplex::Simplex::vertex(vertex)).simplex_vector(PrimitiveType::Vertex).size());
}


namespace detail {

int64_t swap_valence(const TriMesh& mesh, const Tuple& v, bool boundaries)
{
    int64_t val = simplex::valence(mesh,v);
    if (boundaries && mesh.is_boundary(PrimitiveType::Vertex, v)) {
        // TODO: this 2 should be something like 6 * (angle of triangles / 2pi) for planar problems
        val += 2;
    }
    return val;
}
auto swap_valences(const TriMesh& mesh, const Tuple& t, bool boundaries) -> std::array<int64_t, 4>
{
    const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Triangle);
    const static int8_t sv = sd.primitive_as_index(PrimitiveType::Vertex);
    const static int8_t opp = sd.opposite();


    const dart::Dart d0 = sd.dart_from_tuple(t);
    const dart::Dart d1 = sd.act(d0, sv);
    const dart::Dart d2 = sd.act(d0, opp);

    const dart::Dart d3 = sd.act(sd.dart_from_tuple(mesh.switch_face(t)), opp);

    const Tuple v0 = sd.tuple_from_dart(d0);
    const Tuple v1 = sd.tuple_from_dart(d1);
    const Tuple v2 = sd.tuple_from_dart(d2);
    const Tuple v3 = sd.tuple_from_dart(d3);


    int64_t val0 = swap_valence(mesh, v0, boundaries);
    int64_t val1 = swap_valence(mesh, v1, boundaries);
    int64_t val2 = swap_valence(mesh, v2, boundaries);
    int64_t val3 = swap_valence(mesh, v3, boundaries);
    return {{val0, val1, val2, val3}};
}

int64_t valence_before(const TriMesh& mesh, const Tuple& t) 
{
    assert(!mesh.is_boundary( PrimitiveType::Edge, t));


    auto prev = swap_valences(mesh,t);
    return swap_valence_variance(prev);
}
int64_t swap_valence_variance(
    const std::array<int64_t, 4>& valences) 
{
    const auto [val0, val1, val2, val3] = valences;
    // auto [val0, val1, val2, val3] = valences(t);
    // formula from: https://github.com/daniel-zint/hpmeshgen/blob/cdfb9163ed92523fcf41a127c8173097e935c0a3/src/HPMeshGen2/TriRemeshing.cpp#L315
    return std::max(std::abs(val0 - 6), std::abs(val1 - 6)) +
           std::max(std::abs(val2 - 6), std::abs(val3 - 6));
}
int64_t valence_after(const TriMesh& mesh,const Tuple& t) 
{
    assert(!mesh.is_boundary( PrimitiveType::Edge, t));


    auto prev = swap_valences(mesh,t);
    return swap_valence_variance_after(prev);
}
int64_t swap_valence_variance_after(
    const std::array<int64_t, 4>& valences) 
{
    const auto [val0, val1, val2, val3] = valences;
    // formula from: https://github.com/daniel-zint/hpmeshgen/blob/cdfb9163ed92523fcf41a127c8173097e935c0a3/src/HPMeshGen2/TriRemeshing.cpp#L315
    return std::max(std::abs(val0 - 7), std::abs(val1 - 7)) +
           std::max(std::abs(val2 - 5), std::abs(val3 - 5));
}

int64_t swap_valence_variance(const TriMesh& mesh, const Tuple& vertex) {
    return valence_before(mesh,vertex);

}
int64_t swap_valence_variance_after(const TriMesh& mesh, const Tuple& vertex) {
    return valence_after(mesh,vertex);
}

}
} // namespace wmtk::simplex
