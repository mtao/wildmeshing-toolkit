#pragma once

#include <wmtk/Accessor.hpp>
#include <wmtk/simplex/Simplex.hpp>

namespace wmtk::function {

class Function
{
public:
    Function();
    virtual ~Function() {}

    /**
     * @brief Given a function f(x), get_value evaluate the function at the input simplex.
     *
     * @param variable_simplex The input simplex. f(x) is defined wrt this argument.
     * @return double The value of the function at the input simplex.
     */
    virtual double get_value(const simplex::Simplex& simplex) const = 0;


    /**
     * @brief get_gradient evaluates the gradient of the function f(x) defined wrt the variable x.
     *
     * This variable is represented by the input argument variable_simplex.
     *
     * @param variable_simplex The input simplex. f(x) is defined wrt the attributes of this
     * argument.
     * @return Eigen::VectorXd The gradient of the function. The dimension of the vector is equal to
     * the dimension of the embedded space.
     */
    virtual Eigen::VectorXd get_gradient(const simplex::Simplex& variable_simplex) const
    {
        throw std::runtime_error("Gradient not implemented");
    }

    /**
     * @brief get_hessian evaluates the hessian of the function f(x) defined wrt the variable x.
     *
     * This variable is represented by the input argument variable_simplex.
     *
     * @param variable_simplex The input simplex. f(x) is defined wrt the attributes of this
     * argument.
     * @return Eigen::MatrixXd The hessian of the function. The dimension of the matrix is equal to
     * the dimension of the embedded space.
     */
    virtual Eigen::MatrixXd get_hessian(const simplex::Simplex& variable_simplex) const
    {
        throw std::runtime_error("Hessian not implemented");
    }

    virtual const Mesh& mesh() const = 0;
    virtual PrimitiveType primitive_type() const = 0;

    // the
    virtual int64_t embedded_dimension() const = 0;
};
} // namespace wmtk::function
