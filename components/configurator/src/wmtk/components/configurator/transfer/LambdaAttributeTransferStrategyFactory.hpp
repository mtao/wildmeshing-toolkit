#pragma once
#include "SingleAttributeTransferStrategyFactory.hpp"

namespace wmtk::components::configurator::transfer {

template <typename ToType, int ToDim, typename FromType, int FromDim>
struct LambdaFunctionTransferStrategyFactory : public SingleAttributeTransferStrategyFactoryBase
{
    using RetObjectType =
        wmtk::operations::SingleAttributeTransferStrategy<ToType, FromType, ToDim, FromDim>;
    using FunctorType = typename RetObjectType::FunctorType;
    using FunctorWithoutSimplicesType = typename RetObjectType::FunctorWithoutSimplicesType;

    using FunctorVariant = std::variant<FunctorType, FunctorWithoutSimplicesType>;
    LambdaFunctionTransferStrategyFactory(FunctorType f = {})
        : m_functor(f)
    {}
    LambdaFunctionTransferStrategyFactory(FunctorWithoutSimplicesType f)
        : m_functor(f)
    {}
    LambdaFunctionTransferStrategyFactory(FunctorVariant f)
        : m_functor(f)
    {}
    ~LambdaFunctionTransferStrategyFactory() = default;


    components::multimesh::utils::AttributeDescription get_output_attribute_description(
        const wmtk::components::multimesh::MeshCollection&) const final;

    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_transfer(
        wmtk::components::multimesh::MeshCollection& mc) const final;

    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_transfer(
        const attribute::MeshAttributeHandle& to,
        const attribute::MeshAttributeHandle& from) const;

private:
    FunctorVariant m_functor;
};

template <typename ToType, int ToDim, typename FromType, int FromDim>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::create_transfer(
    const attribute::MeshAttributeHandle& to,
    const attribute::MeshAttributeHandle& from) const
{
    return std::visit(
        [&](const auto& f) {
            return std::make_shared<
                wmtk::operations::
                    SingleAttributeTransferStrategy<ToType, FromType, ToDim, FromDim>>(to, from, f);
        },
        m_functor);
}

template <typename ToType, int ToDim, typename FromType, int FromDim>
components::multimesh::utils::AttributeDescription
LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::
    get_output_attribute_description(const wmtk::components::multimesh::MeshCollection& mc) const
{
    auto attr = attribute;
    assert(attr.fully_specified());
    return attr;
}
template <typename ToType, int ToDim, typename FromType, int FromDim>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::create_transfer(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    auto from_attr = wmtk::components::multimesh::utils::get_attribute(mc, base_attribute());
    auto to_attr_d = get_output_attribute_description(mc);

    auto to_attr = wmtk::components::multimesh::utils::create_attribute(mc, to_attr_d);

    return create_transfer(to_attr, from_attr);
}

} // namespace wmtk::components::configurator::transfer
