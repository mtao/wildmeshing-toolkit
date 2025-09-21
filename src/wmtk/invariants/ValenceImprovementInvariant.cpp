#include "ValenceImprovementInvariant.hpp"

#include <wmtk/TriMesh.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/simplex/faces_single_dimension.hpp>
#include <wmtk/simplex/link.hpp>


namespace wmtk::invariants {
namespace {
const auto& sd = dart::SimplexDart::get_singleton(PrimitiveType::Triangle);

}
const TriMesh& ValenceImprovementInvariant::mesh() const
{
    return static_cast<const TriMesh&>(Invariant::mesh());
}

ValenceImprovementInvariant::ValenceImprovementInvariant(const TriMesh& m)
    : Invariant(m, true, false, false)
{}

int64_t ValenceImprovementInvariant::pre_valence(const Tuple& v) const
{
    int64_t val = simplex::link(mesh(), simplex::Simplex::vertex(mesh(), v))
                      .simplex_vector(PrimitiveType::Vertex)
                      .size();
    if (mesh().is_boundary(PrimitiveType::Vertex, v)) {
        val += 2;
    }
    return val;
}

auto ValenceImprovementInvariant::pre_valences(const Tuple& t) const -> std::array<int64_t, 4>
{
    const static int8_t sv = sd.primitive_as_index(PrimitiveType::Vertex);
    const static int8_t opp = sd.opposite();


    const dart::Dart d0 = sd.dart_from_tuple(t);
    const dart::Dart d1 = sd.act(d0, sv);
    const dart::Dart d2 = sd.act(d0, opp);

    const dart::Dart d3 = sd.act(sd.dart_from_tuple(mesh().switch_face(t)), opp);

    const Tuple v0 = sd.tuple_from_dart(d0);
    const Tuple v1 = sd.tuple_from_dart(d1);
    const Tuple v2 = sd.tuple_from_dart(d2);
    const Tuple v3 = sd.tuple_from_dart(d3);


    int64_t val0 = pre_valence(v0);
    int64_t val1 = pre_valence(v1);
    int64_t val2 = pre_valence(v2);
    int64_t val3 = pre_valence(v3);
    return {{val0, val1, val2, val3}};
}


bool ValenceImprovementInvariant::before(const simplex::Simplex& simplex) const
{
    if (mesh().is_boundary(simplex)) {
        return false;
    }
    const Tuple& t = simplex.tuple();

    assert(simplex.primitive_type() == PrimitiveType::Edge);

    // auto [val0, val1, val2, val3] = pre_valences(t);
    const auto prev = pre_valences(t);

    const int64_t val_before = valence_before(prev);
    const int64_t val_after = valence_after_(prev);


    bool ret = val_after < val_before;
    return ret;
}

int64_t ValenceImprovementInvariant::valence_before(const simplex::Simplex& s) const
{
    assert(!mesh().is_boundary(s));
    return false;
    const Tuple& t = s.tuple();

    assert(s.primitive_type() == PrimitiveType::Edge);

    auto prev = pre_valences(t);
    return valence_before(prev);
}
int64_t ValenceImprovementInvariant::valence_before(
    const std::array<int64_t, 4>& pre_valences) const
{
    const auto [val0, val1, val2, val3] = pre_valences;
    // auto [val0, val1, val2, val3] = pre_valences(t);
    // formula from: https://github.com/daniel-zint/hpmeshgen/blob/cdfb9163ed92523fcf41a127c8173097e935c0a3/src/HPMeshGen2/TriRemeshing.cpp#L315
    return std::max(std::abs(val0 - 6), std::abs(val1 - 6)) +
           std::max(std::abs(val2 - 6), std::abs(val3 - 6));
}
int64_t ValenceImprovementInvariant::valence_after(const simplex::Simplex& s) const
{
    assert(!mesh().is_boundary(s));
    return false;
    const Tuple& t = s.tuple();

    assert(s.primitive_type() == PrimitiveType::Edge);

    auto prev = pre_valences(t);
    return valence_after_(prev);
}
int64_t ValenceImprovementInvariant::valence_after_(
    const std::array<int64_t, 4>& pre_valences) const
{
    const auto [val0, val1, val2, val3] = pre_valences;
    // formula from: https://github.com/daniel-zint/hpmeshgen/blob/cdfb9163ed92523fcf41a127c8173097e935c0a3/src/HPMeshGen2/TriRemeshing.cpp#L315
    return std::max(std::abs(val0 - 7), std::abs(val1 - 7)) +
           std::max(std::abs(val2 - 5), std::abs(val3 - 5));
}


std::string ValenceImprovementInvariant::name() const
{
    return "ValenceImprovementInvariant";
}
} // namespace wmtk::invariants
