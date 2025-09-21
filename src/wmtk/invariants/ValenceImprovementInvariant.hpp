#pragma once

#include <wmtk/attribute/AttributeHandle.hpp>
#include "Invariant.hpp"

namespace wmtk {
class TriMesh;
}
namespace wmtk::invariants {
class ValenceImprovementInvariant : public Invariant
{
public:
    ValenceImprovementInvariant(const TriMesh& m);


    const TriMesh& mesh() const;
    bool before(const simplex::Simplex& t) const override;
    std::string name() const override;


    // evaluates the valence of a triangle before a swap.
    int64_t valence_before(const simplex::Simplex& s) const;
    // evaluates the valence of a triangle after a swap
    int64_t valence_after(const simplex::Simplex& s) const;

private:
    std::array<int64_t, 4> pre_valences(const Tuple& t) const;
    int64_t valence_before(const std::array<int64_t, 4>& pre_valences) const;
    // TODO: why was polymorphism failing here? (internally valence_after was only catching
    // valence_after(simplex)
    int64_t valence_after_(const std::array<int64_t, 4>& pre_valences) const;
    int64_t pre_valence(const Tuple& t) const;
};
} // namespace wmtk::invariants
