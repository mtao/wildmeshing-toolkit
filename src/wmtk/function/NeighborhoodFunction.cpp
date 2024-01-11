#pragma once

#include "NeighborhoodFunction.hpp"
#include "PerSimplexFunction.hpp"

namespace wmtk::function {


NeighborhoodFunction::NeighborhoodFunction(std::shared_ptr<PerSimplexFunction> function)
    : m_function(function)
{}


const PerSimplexFunction& NeighborhoodFunction::function() const
{
    return *m_function;
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
    return function().mesh();
}
PrimitiveType AttributeFunction::primitive_type() const
{
    return function().primitive_type();
};
} // namespace wmtk::function
