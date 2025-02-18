#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "TransferStrategyFactory.hpp"

#include <wmtk/Types.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/create_attribute.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>

namespace wmtk::components::mesh_info::transfer {

struct SingleAttributeTransferStrategyFactoryBase : public TransferStrategyFactory
{
    SingleAttributeTransferStrategyFactoryBase();
    ~SingleAttributeTransferStrategyFactoryBase();
    std::string base_attribute_path;
    int8_t simplex_dimension;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json& j) final;

    // entry point to set the type and dim of the output
    attribute::AttributeType output_type(wmtk::components::multimesh::MeshCollection& mc) const;
    // entry point to set the type and dim of the output
    int output_dimension(wmtk::components::multimesh::MeshCollection& mc) const;
};

template <template <typename, int, typename, int> typename Functor>
struct SingleAttributeTransferStrategyFactory : public SingleAttributeTransferStrategyFactoryBase
{
    SingleAttributeTransferStrategyFactory();
    ~SingleAttributeTransferStrategyFactory();

    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_transfer(
        wmtk::components::multimesh::MeshCollection& mc) const final;
    // std::unique_ptr<TransferStrategyFactory> clone() const final;

    template <int ToDim, int FromDim, typename ToT, typename FromT>
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create_T(
        const attribute::MeshAttributeHandle& to,
        const attribute::MeshAttributeHandle& from) const;
};

template <template <typename, int, typename, int> typename Functor>
template <int ToDim, int FromDim, typename ToT, typename FromT>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
SingleAttributeTransferStrategyFactory<Functor>::create_T(
    const attribute::MeshAttributeHandle& to,
    const attribute::MeshAttributeHandle& from) const
{
    using F = Functor<FromT, FromDim, ToT, ToDim>;
    if constexpr (F::valid()) {
        return std::make_shared<
            wmtk::operations::SingleAttributeTransferStrategy<ToT, FromT, ToDim, FromDim>>(
            to,
            from,
            [](auto&& a) { return F::execute(a); });
    } else {
        assert(false);
        return {};
    }
}

template <template <typename, int, typename, int> typename Functor>
std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>
SingleAttributeTransferStrategyFactory<Functor>::create_transfer(
    wmtk::components::multimesh::MeshCollection& mc) const
{
    auto from_attr = wmtk::components::multimesh::utils::get_attribute(mc, {base_attribute_path});
    auto to_attr = wmtk::components::multimesh::utils::create_attribute(
        mc,
        {attribute_path, simplex_dimension, output_type(mc), output_dimension(mc)});


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


} // namespace wmtk::components::mesh_info::transfer
