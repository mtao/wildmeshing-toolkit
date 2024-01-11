#pragma once

#include "PerSimplexFunction.hpp"

namespace wmtk::function {

class PerSimplexFunction;

// this abstraction supports LocalNeighborSumFunction, WeightedLocalNeighborSumFunction,
// LocalNeighborhoodMaxFunction, which were all requested recently
class LocalNeighborhoodFunction : public Function
{
public:
    LocalNeighborhoodFunction(std::shared_ptr<PerSimplexFunction> function);


    virtual std::vector<simplex::Simplex> domain(
        const simplex::Simplex& variable_simplex) const = 0;

    const PerSimplexFunction& function() const { return *m_function; }
    const Mesh& mesh() const override;
    PrimitiveType primitive_type() const override;

    // the
    virtual int64_t embedded_dimension() const override;

private:
    std::shared_ptr<PerSimplexFunction> m_function;
};
} // namespace wmtk::function
