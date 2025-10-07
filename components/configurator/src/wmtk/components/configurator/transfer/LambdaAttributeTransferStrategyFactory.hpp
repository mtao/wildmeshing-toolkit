#pragma once
#include "SingleAttributeTransferStrategyFactory.hpp"

namespace wmtk::components::configurator::transfer {

template <typename InType, int InDim, typename OutType, int OutDim>
struct LambdaFunctionTransferStrategyFactory : public SingleAttributeTransferStrategyFactoryBase
{
    using RetObjectType =
        wmtk::operations::SingleAttributeTransferStrategy<InType, OutType, InDim, OutDim>;
    using FunctorType = typename RetObjectType::FunctorType;
    using FunctorWithoutSimplexType = typename RetObjectType::FunctorWithoutSimplexType;
    LambdaFunctionTransferStrategyFactory(FunctorType f = {})
        : m_functor(f)
    {}
    LambdaFunctionTransferStrategyFactory(FunctorWithoutSimplexType f)
        : m_functor(f)
    {}
    ~LambdaFunctionTransferStrategyFactory();


    components::multimesh::utils::AttributeDescription get_output_attribute_description(
        const wmtk::components::multimesh::MeshCollection&) const final;

    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_transfer(
        wmtk::components::multimesh::MeshCollection& mc) const final;

    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_transfer(
        const attribute::MeshAttributeHandle& to,
        const attribute::MeshAttributeHandle& from) const;

private:
    std::variant<FunctorType, FunctorWithoutSimplexType> m_functor;
};

template <typename InType, int InDim, typename OutType, int OutDim>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
LambdaFunctionTransferStrategyFactory<InType, InDim, OutType, OutDim>::create_transfer(
    const attribute::MeshAttributeHandle& to,
    const attribute::MeshAttributeHandle& from) const
{
    return std::visit(
        [&](const auto& f) {
            return std::make_shared<
                wmtk::operations::SingleAttributeTransferStrategy<InType, OutType, InDim, OutDim>>(
                to,
                from,
                f);
        },
        m_functor);
}

template <typename InType, int InDim, typename OutType, int OutDim>
components::multimesh::utils::AttributeDescription
LambdaFunctionTransferStrategyFactory<InType, InDim, OutType, OutDim>::
    get_output_attribute_description(const wmtk::components::multimesh::MeshCollection& mc) const
{
    auto attr = base_attribute();
    assert(attr.fully_specified());
    return attr;
}
template <typename InType, int InDim, typename OutType, int OutDim>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
LambdaFunctionTransferStrategyFactory<InType, InDim, OutType, OutDim>::create_transfer(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    auto from_attr = wmtk::components::multimesh::utils::get_attribute(mc, base_attribute());
    auto to_attr_d = get_output_attribute_description(mc);

    auto to_attr = wmtk::components::multimesh::utils::create_attribute(mc, to_attr_d);

    return create_transfer(to_attr, from_attr);
}

} // namespace wmtk::components::configurator::transfer
