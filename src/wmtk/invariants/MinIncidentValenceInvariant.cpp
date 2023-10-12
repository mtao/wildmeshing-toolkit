#include "MinIncidentValenceInvariant.hpp"

#include <wmtk/Mesh.hpp>
#include <wmtk/SimplicialComplex.hpp>

namespace wmtk::invariants {


MinIncidentValenceInvariant::MinIncidentValenceInvariant(const Mesh& m, long min_valence)
    : MeshInvariant(m)
    , m_min_valence(min_valence)
{}

bool MinIncidentValenceInvariant::before(const Tuple& t) const
{
    const Tuple v0 = t;
    const Tuple v1 = mesh().switch_vertex(t);
    const long val0 = static_cast<long>(SimplicialComplex::vertex_one_ring(mesh(), v0).size());
    const long val1 = static_cast<long>(SimplicialComplex::vertex_one_ring(mesh(), v1).size());

    return val0 >= m_min_valence && val1 >= m_min_valence;
}

bool MinIncidentValenceInvariant::after(PrimitiveType type, const std::vector<Tuple>& t) const
{
    assert(type == PrimitiveType::Edge);

    for (const Tuple& e : t) {
        if (!before(e)) {
            return false;
        }
    }

    return true;
}

} // namespace wmtk::invariants