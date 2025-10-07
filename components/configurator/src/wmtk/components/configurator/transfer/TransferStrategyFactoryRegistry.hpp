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

    template <typename InType, int InDim, typename OutType, int OutDim, typename Func>
    void register_lambda_transfer(const std::string_view& name, Func&& f = {});

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

template <typename InType, int InDim, typename OutType, int OutDim, typename Func>
void TransferStrategyFactoryRegistry::register_lambda_transfer(
    const std::string_view& name,
    Func&& f)
{
    if (!has(name)) {
        m_map.emplace(name, [f](const TransferStrategyOptions& js) {
            auto t = std::make_shared<
                LambdaFunctionTransferStrategyFactory<InType, InDim, OutType, OutDim>>(f);
            t->from_options(js);
            return t;
        });
    }
}
} // namespace wmtk::components::configurator::transfer

