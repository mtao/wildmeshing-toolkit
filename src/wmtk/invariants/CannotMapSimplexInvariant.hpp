#pragma once

#include <wmtk/invariants/Invariant.hpp>

namespace wmtk::invariants {
/**
 * This invariant returns true if the given simplex, or one of its neighbors can be mapped to a
 * child mesh. (neighbor is faces or cofaces. That is, if a is a neighbor of b then 
 *  a \cap b == a or b
 */
class CannotMapSimplexInvariant : public Invariant
{
public:
    CannotMapSimplexInvariant(const Mesh& parent_mesh, const Mesh& child_mesh);
    CannotMapSimplexInvariant(
        const Mesh& parent_mesh,
        const Mesh& child_mesh,
        PrimitiveType mapped_simplex_type,
        bool invert = false);
    // main entrypoint
    bool before(const simplex::Simplex& s) const override;

    // entry for checking two simplices of the same type
    bool before_same_dimension(const simplex::Simplex& s) const;
    bool before_default(const simplex::Simplex& s) const;
    bool before_same_dimension_vertex(const simplex::Simplex& s) const;
    bool before_same_dimension_edge(const simplex::Simplex& s) const;

    std::string name() const override;
private:
    const Mesh& m_child_mesh;
    bool inverse = false;
    // the type of primitive we are blocking if one exists nearby
    PrimitiveType m_mapped_simplex_type;
};
} // namespace wmtk::invariants
