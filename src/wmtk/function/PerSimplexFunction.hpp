#pragma once

#include <wmtk/Primitive.hpp>

#include <wmtk/attribute/MeshAttributes.hpp>

#include <Eigen/Core>
#include "AttributeFunction.hpp"

namespace wmtk::function {

class PerSimplexFunction
{
public:
    PerSimplexFunction(
        const Mesh& domain_mesh,
        const PrimitiveType domain_primitive_type,
        const attribute::MeshAttributeHandle& variable_attribute_handle);
    virtual ~PerSimplexFunction() {}

    /**
     * @brief This function is defined over a simplex (normally a triangle or tetrahedron). And the
     * domain of the function is represented by the input argument domain_simplex.
     *
     * @param domain_simplex The domain that the function is defined over.
     * @return double The numerical value of the function at the input domain.
     */
    virtual double get_value(
        const simplex::Simplex& domain_simplex,
        const simplex::Simplex& variable_simplex) const = 0;
    virtual Eigen::VectorXd get_gradient(
        const simplex::Simplex& domain_simplex,
        const simplex::Simplex& variable_simplex) const
    {
        throw std::runtime_error("Gradient not implemented");
    }
    virtual Eigen::MatrixXd get_hessian(
        const simplex::Simplex& domain_simplex,
        const simplex::Simplex& variable_simplex) const
    {
        throw std::runtime_error("Hessian not implemented");
    }

private:
    attribute::MeshAttributeHandle m_variable_handle;

protected:
    const Mesh& m_domain_mesh;
    const PrimitiveType m_domain_primitive_type;
};
} // namespace wmtk::function
