#pragma once

#include "Invariant.hpp"

namespace wmtk {
namespace invariants {

    // invokes the link condition on each mesh in a multimesh
    // only works for edge meshes
class MultiMeshLinkConditionInvariant : public Invariant
{
public:
    MultiMeshLinkConditionInvariant(const Mesh& m);
    bool before(const simplex::Simplex& t) const override;

    std::string name() const override;
};
} // namespace invariants
using MultiMeshLinkConditionInvariant = invariants::MultiMeshLinkConditionInvariant;
} // namespace wmtk
