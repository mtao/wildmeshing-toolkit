#pragma once

#include <wmtk/attribute/TypedAttributeHandle.hpp>
#include "Invariant.hpp"

namespace wmtk {
class Swap32EnergyBeforeInvariant : public Invariant
{
public:
    Swap32EnergyBeforeInvariant(
        const Mesh& m,
        const attribute::TypedAttributeHandle<Rational>& coordinate,
        double eps = 1.0001);
    using Invariant::Invariant;

    bool before(const simplex::Simplex& t) const override;

private:
    const attribute::TypedAttributeHandle<Rational> m_coordinate_handle;
    const double m_eps;
};
} // namespace wmtk
