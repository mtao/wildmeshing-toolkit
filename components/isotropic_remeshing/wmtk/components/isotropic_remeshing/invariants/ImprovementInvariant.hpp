#pragma once

#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/invariants/Invariant.hpp>

namespace wmtk::components::isotropic_remeshing::invariants {

/**
 * Compute the distance from the center of the quadric-projected edge (both vertices moved to the
 * quadric minimum) to the edge's quadric minimum. Compare this distance with the given value
 */
class ImprovementInvariant : public wmtk::invariants::Invariant
{
public:
    ImprovementInvariant(const attribute::MeshAttributeHandle& attribute_handle);

    bool after(
        const std::vector<Tuple>& top_dimension_tuples_before,
        const std::vector<Tuple>& top_dimension_tuples_after) const final;

private:
    const attribute::MeshAttributeHandle m_attribute;
};

} // namespace wmtk::components::isotropic_remeshing::invariants
