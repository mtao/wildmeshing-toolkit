#include "LocalNeighborsSumFunction.hpp"

#include "PerSimplexFunction.hpp"

#include <wmtk/simplex/cofaces_single_dimension.hpp>


namespace wmtk::function {

namespace {
template <typename
}

std::vector<simplex::Simplex> LocalNeighborsSumFunction::domain(
    const simplex::Simplex& variable_simplex) const
{
    return wmtk::simplex::cofaces_single_dimension_simplices(
        mesh(),
        variable_simplex,
        m_domain_simplex_type);
}


template <typename ReturnType, typename Func>
ReturnType LocalNeighborsSumFunction::get(
    Func&& f,
    const simplex::Simplex& variable_simplex,
    Zero&& zero) const
{
    const auto neighs = domain(variable_simplex);
    assert(variable_simplex.primitive_type() == attribute_type());

    // assert(attribute_handle() == m_function.attribute_handle());

    ReturnType t = zero;
    for (const simplex::Simplex& cell : neighs) {
        assert(cell.primitive_type() == m_domain_simplex_type);
        res += func(cell);
    }

    return res;
}

double LocalNeighborsSumFunction::get_value(const simplex::Simplex& variable_simplex) const
{
    return get<double>(
        [&](const simplex::Simplex& domain_simplex) {
            return function().get_value(domain_simplex, variable_simplex);
        },
        variable_simplex,
        0.0);
}

Eigen::VectorXd LocalNeighborsSumFunction::get_gradient(
    const simplex::Simplex& variable_simplex) const
{
    return get<Eigen::VectorXd>(
        [&](const simplex::Simplex& domain_simplex) {
            return function().get_gradient(domain_simplex, variable_simplex);
        },
        variable_simplex,
        Eigen::VectorXd::Zero(embedded_dimension()));
}

Eigen::MatrixXd LocalNeighborsSumFunction::get_hessian(
    const simplex::Simplex& variable_simplex) const
{
    return get<Eigen::MatrixXd>(
        [&](const simplex::Simplex& domain_simplex) -> double {
            return function().get_gradient(domain_simplex, variable_simplex);
        },
        variable_simplex,
        Eigen::MatrixXd::Zero(embedded_dimension(), embedded_dimension()));
}

} // namespace wmtk::function
