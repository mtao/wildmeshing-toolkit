#pragma once

#include <wmtk/Primitive.hpp>

#include <wmtk/attribute/MeshAttributes.hpp>

#include <Eigen/Core>

namespace wmtk::function {

/* @brief a function whose argument is the value of an attribute
 *
 */
class AttributeFunction : public Function
{
public:
    AttributeFunction(
        const Mesh& domain_mesh,
        const attribute::MeshAttributeHandle& variable_attribute_handle);
    virtual ~AttributeFunction() {}


    // if this function is f(x), this returns the dimension of x
    virtual double get_value(const simplex::Simplex& simplex) const = 0;


    const Mesh& mesh() const override;
    PrimitiveType primitive_type() const override;

    // the
    int64_t embedded_dimension() const override;


private:
    attribute::MeshAttributeHandle m_handle;

protected:
    const Mesh& m_domain_mesh;
    const PrimitiveType m_domain_primitive_type;
};
} // namespace wmtk::function
