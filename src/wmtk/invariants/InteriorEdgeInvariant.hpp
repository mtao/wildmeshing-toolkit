#pragma once

#include "InteriorSimplexInvariant.hpp"

namespace wmtk {
namespace invariants {
class InteriorEdgeInvariant : public InteriorSimplexInvariant
{
public:
    InteriorEdgeInvariant(const Mesh& m);
    virtual std::string name() const override;
};
} // namespace invariants
using InteriorEdgeInvariant = invariants::InteriorEdgeInvariant;
} // namespace wmtk
