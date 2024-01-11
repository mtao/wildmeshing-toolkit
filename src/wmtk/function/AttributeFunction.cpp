
#include "AttributeFunction.hpp"

#include <wmtk/Mesh.hpp>

namespace wmtk::function {

AttributeFunction::AttributeFunction(const attribute::MeshAttributeHandle& handle)
    : m_handle(handle)
{
    assert(handle.holds<double>());
}

int64_t AttributeFunction::embedded_dimension() const
{
    assert(m_handle.is_valid());
    auto res = mesh().get_attribute_dimension(m_handle.as<double>());
    assert(res > 0);
    return res;
}

const Mesh& AttributeFunction::mesh() const
{
    return m_mesh;
}
PrimitiveType AttributeFunction::primitive_type() const
{
    return m_handle.primitive_type();
};
const attribute::MeshAttributeHandle& AttributeFunction::attribute_handle() const
{
    return m_handle;
}

} // namespace wmtk::function
