#include "ValenceImprovementInvariant.hpp"

#include <wmtk/TriMesh.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/simplex/faces_single_dimension.hpp>
#include <wmtk/simplex/link.hpp>
#include <wmtk/simplex/valence.hpp>


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


bool ValenceImprovementInvariant::before(const simplex::Simplex& simplex) const
{
    if (mesh().is_boundary(simplex)) {
        return false;
    }
    const Tuple& t = simplex.tuple();

    assert(simplex.primitive_type() == PrimitiveType::Edge);

    // auto [val0, val1, val2, val3] = pre_valences(t);
    const auto prev = simplex::detail::swap_valences(mesh(), t);

    const int64_t val_before = simplex::detail::swap_valence_variance(prev);
    const int64_t val_after = simplex::detail::swap_valence_variance_after(prev);


    bool ret = val_after < val_before;
    return ret;
}


std::string ValenceImprovementInvariant::name() const
{
    return "ValenceImprovementInvariant";
}
} // namespace wmtk::invariants
