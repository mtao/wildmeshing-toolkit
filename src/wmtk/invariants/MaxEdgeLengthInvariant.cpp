#include "MaxEdgeLengthInvariant.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/simplex/Simplex.hpp>

namespace wmtk {
MaxEdgeLengthInvariant::MaxEdgeLengthInvariant(
    const Mesh& m,
    const TypedAttributeHandle<double>& coordinate,
    double threshold_squared)
    : Invariant(m, true, false, false)
    , m_coordinate_handle(coordinate)
    , m_threshold_squared(threshold_squared)
{}
bool MaxEdgeLengthInvariant::before(const simplex::Simplex& t) const
{
    const attribute::Accessor<double> accessor = mesh().create_const_accessor(m_coordinate_handle);

    auto p0 = accessor.const_vector_attribute(t.tuple());
    auto p1 = accessor.const_vector_attribute(mesh().switch_tuple(t.tuple(), PrimitiveType::Vertex));
    const double l_squared = (p1 - p0).squaredNorm();
    return l_squared < m_threshold_squared;
}
std::string MaxEdgeLengthInvariant::name() const
{
    return "MaxEdgeLengthInvariant";
}
} // namespace wmtk
