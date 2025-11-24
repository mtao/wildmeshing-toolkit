#pragma once
#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include "TransferStrategyOptions.hpp"
#include "wmtk/components/configurator/transfer/LambdaAttributeTransferStrategyFactory.hpp"

namespace wmtk::components::configurator::transfer {
struct TransferStrategyFactory;
struct TransferStrategyOptions;

// registry holding the
class TransferStrategyFactoryRegistry
{
public:
    bool has(const std::string_view& name) const;
    template <typename Type>
    void register_transfer(const std::string_view& name);
    std::shared_ptr<TransferStrategyFactory> create(
        const std::string_view& type,
        const TransferStrategyOptions&) const;
    std::shared_ptr<TransferStrategyFactory> create(const TransferStrategyOptions&) const;

    std::vector<std::string> names() const;

    template <typename ToType, int ToDim, typename FromType, int FromDim>
    void register_lambda_transfer(
        const std::string_view& name,
        typename LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::
            FunctorType f = {});

    template <typename ToType, int ToDim, typename FromType, int FromDim>
    void register_lambda_transfer_without_simplices(
        const std::string_view& name,
        typename LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::
            FunctorWithoutSimplicesType f = {});

private:
    std::map<
        std::string,
        std::function<std::shared_ptr<TransferStrategyFactory>(const TransferStrategyOptions&)>>
        m_map;
};

template <typename Type>
void TransferStrategyFactoryRegistry::register_transfer(const std::string_view& name)
{
    if (!has(name)) {
        m_map.emplace(name, [](const TransferStrategyOptions& js) {
            auto t = std::make_shared<Type>();
            t->from_options(js);
            return t;
        });
    }
}

template <typename ToType, int ToDim, typename FromType, int FromDim>
void TransferStrategyFactoryRegistry::register_lambda_transfer_without_simplices(
    const std::string_view& name,
    typename LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::
        FunctorWithoutSimplicesType f)
{
    if (!has(name)) {
        m_map.emplace(name, [f](const TransferStrategyOptions& js) {
            auto t = std::make_shared<
                LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>>(f);
            t->from_options(js);
            spdlog::info("lambda options {}");
            return t;
        });
    }
}

template <typename ToType, int ToDim, typename FromType, int FromDim>
void TransferStrategyFactoryRegistry::register_lambda_transfer(
    const std::string_view& name,
    typename LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>::FunctorType f)
{
    if (!has(name)) {
        m_map.emplace(name, [f](const TransferStrategyOptions& js) {
            auto t = std::make_shared<
                LambdaFunctionTransferStrategyFactory<ToType, ToDim, FromType, FromDim>>(f);
            t->from_options(js);
            return t;
        });
    }
}
} // namespace wmtk::components::configurator::transfer

