#include "InteriorEdgeInvariant.hpp"
#include <wmtk/Mesh.hpp>

namespace wmtk::invariants {
InteriorEdgeInvariant::InteriorEdgeInvariant(const Mesh& m)
    : InteriorSimplexInvariant(m, PrimitiveType::Edge)
{}
std::string InteriorEdgeInvariant::name() const
{
    return "InteriorEdgeInvariant";
}
} // namespace wmtk::invariants
