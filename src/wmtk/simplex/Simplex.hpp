#pragma once

#include <wmtk/Primitive.hpp>
#include <wmtk/Tuple.hpp>

namespace wmtk::simplex {

class Simplex
{
    PrimitiveType m_primitive_type;
    Tuple m_tuple;

public:
    Simplex(const PrimitiveType& ptype, const Tuple& t)
        : m_primitive_type{ptype}
        , m_tuple{t}
    {}

    PrimitiveType primitive_type() const { return m_primitive_type; }
    long dimension() const { return get_simplex_dimension(m_primitive_type); }
    const Tuple& tuple() const { return m_tuple; }

    static Simplex vertex(const Tuple& t) { return Simplex(PrimitiveType::Vertex, t); }
    static Simplex edge(const Tuple& t) { return Simplex(PrimitiveType::Edge, t); }
    static Simplex face(const Tuple& t) { return Simplex(PrimitiveType::Face, t); }
    static Simplex tetrahedron(const Tuple& t) { return Simplex(PrimitiveType::Tetrahedron, t); }

    bool operator==(const Simplex& o) const;
    bool operator<(const Simplex& o) const;
};
} // namespace wmtk::simplex
