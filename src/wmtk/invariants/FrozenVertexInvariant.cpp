#include "FrozenVertexInvariant.hpp"
#include <wmtk/Mesh.hpp>

namespace wmtk::invariants {
FrozenVertexInvariant::FrozenVertexInvariant(
    const Mesh& m,
    const TypedAttributeHandle<int64_t>& frozen_vertex_handle, bool opposite_vertex)
    : Invariant(m, true, false, false)
    , m_frozen_vertex_handle(frozen_vertex_handle)
      , m_do_opposite_vertex(opposite_vertex)
{}

bool FrozenVertexInvariant::before(const simplex::Simplex& t) const
{
    const auto accessor = mesh().create_const_accessor(m_frozen_vertex_handle);

    if (accessor.const_scalar_attribute(simplex::Simplex::vertex(mesh(), t.tuple())) == 1) {
        return false;
    }
    return true;
}


std::string FrozenVertexInvariant::name() const
{
    return "FrozenVertexInvariant";
}
} // namespace wmtk::invariants
