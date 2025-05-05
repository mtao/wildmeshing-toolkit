#pragma once

#include "InteriorSimplexInvariant.hpp"

namespace wmtk {
namespace invariants {
class InteriorVertexInvariant : public InteriorSimplexInvariant
{
public:
    InteriorVertexInvariant(const Mesh& m);
    virtual std::string name() const override;
};
} // namespace invariants
using InteriorVertexInvariant = invariants::InteriorVertexInvariant;
} // namespace wmtk
