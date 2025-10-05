
#pragma once
#include <nlohmann/json.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/Rational.hpp>
#include "TransferFunctorTraits.hpp"
#include "TransferStrategyFactory.hpp"

#include <wmtk/Types.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/create_attribute.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>

namespace wmtk::components::configurator::transfer {

struct LambdaFunctionTransferStrategyFactoryBase : public TransferStrategyFactory
{
    LambdaFunctionTransferStrategyFactoryBase();
    ~LambdaFunctionTransferStrategyFactoryBase();
    multimesh::utils::AttributeDescription base_attribute() const;
    void set_base_attribute(const multimesh::utils::AttributeDescription&);

    nlohmann::json parameters;

    TransferStrategyOptions to_options() const final;
    void from_options(const TransferStrategyOptions&) final;
};

template <typename InType, int InDim, typename OutType, int OutDim>
struct LambdaFunctionTransferStrategyFactory : public LambdaFunctionTransferStrategyFactoryBase
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

template <template <typename, int, typename, int> typename Functor>
components::multimesh::utils::AttributeDescription
LambdaFunctionTransferStrategyFactory<Functor>::get_output_attribute_description(
    const wmtk::components::multimesh::MeshCollection& mc) const
{
    auto from_attr = wmtk::components::multimesh::utils::get_attribute(mc, base_attribute());
    using Traits = TransferFunctorTraits<Functor>;
    multimesh::utils::AttributeDescription out_type = {
        attribute.path,
        Traits::simplex_dimension(from_attr, parameters),
        Traits::output_type(from_attr, parameters),
        Traits::output_dimension(from_attr)};
    if (!attribute.compatible(out_type)) {
        logger().warn(
            "Attribute transfer from {} to {} was misconfigured because the target attribute is "
            "not compatible with {}",
            base_attribute(),
            attribute,
            out_type);
    }
    return out_type;
}
template <template <typename, int, typename, int> typename Functor>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
LambdaFunctionTransferStrategyFactory<Functor>::create_transfer(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    auto from_attr = wmtk::components::multimesh::utils::get_attribute(mc, base_attribute());
    auto to_attr_d = get_output_attribute_description(mc);

    auto to_attr = wmtk::components::multimesh::utils::create_attribute(mc, to_attr_d);


    return std::visit(
        [&](const auto& to_t, const auto& from_t) noexcept
            -> std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> {
            using FromT = typename std::decay_t<decltype(from_t)>::Type;
            using ToT = typename std::decay_t<decltype(to_t)>::Type;
            auto run = [&](auto&& dim)

                -> std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> {
                constexpr static int d = std::decay_t<decltype(dim)>::value;
                switch (from_attr.dimension()) {
                case 1: return create_T<d, 1, ToT, FromT>(to_attr, from_attr);
                case 2: return create_T<d, 2, ToT, FromT>(to_attr, from_attr);
                case 3: return create_T<d, 3, ToT, FromT>(to_attr, from_attr);
                default: return create_T<d, Eigen::Dynamic, ToT, FromT>(to_attr, from_attr);
                }
            };

            switch (to_attr.dimension()) {
            case 1: return run(std::integral_constant<int, 1>{});
            case 2: return run(std::integral_constant<int, 2>{});
            case 3: return run(std::integral_constant<int, 3>{});
            default: return run(std::integral_constant<int, Eigen::Dynamic>{});
            }
        },
        to_attr.handle(),
        from_attr.handle());
}

} // namespace wmtk::components::configurator::transfer
