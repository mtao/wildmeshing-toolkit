#pragma once

#include "Invariant.hpp"
namespace wmtk {
class TetMesh;
}

namespace wmtk::invariants {

/**
 * @brief for edge swap in tetmesh. Checks that the tet-valence of the edge is equal to the given
 * valence.
 *
 */
class EdgeValenceInvariant : public Invariant
{
public:
    EdgeValenceInvariant(const TetMesh& m, int64_t target_valence);

    bool before(const simplex::Simplex& t) const override;

private:
    int64_t m_valence;
};

} // namespace wmtk::invariants
