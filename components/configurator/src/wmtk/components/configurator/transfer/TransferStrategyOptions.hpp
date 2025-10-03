#pragma once
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/utils/json_macros.hpp>


namespace wmtk::components::configurator::transfer {
struct TransferStrategyOptions
{
    // the attribute that will be written to. This does not need to be fully specified, as the base
    // attribute might have constraints on what this output can be. However, a warning will be made
    // to make sure that it is compatible if specified
    multimesh::utils::AttributeDescription attribute;
    std::string type;

    bool operator<=>(const TransferStrategyOptions&) const = default;
    nlohmann::json parameters;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(TransferStrategyOptions)
};

template <typename ParametersType>
struct TypedTransferStrategyOptions : public TransferStrategyOptions
{
    ParametersType get_parameters() const { return parameters.get<ParametersType>(); }
    void set_parameters(const ParametersType& p) const { parameters = p; }
    TypedTransferStrategyOptions() = default;
    TypedTransferStrategyOptions(const TypedTransferStrategyOptions& opts) = default;
    TypedTransferStrategyOptions(TypedTransferStrategyOptions&& opts) = default;
    TypedTransferStrategyOptions& operator=(const TypedTransferStrategyOptions& opts) = default;
    TypedTransferStrategyOptions& operator=(TypedTransferStrategyOptions&& opts) = default;
    TypedTransferStrategyOptions(const TransferStrategyOptions& o)
        : TransferStrategyOptions(o)
    {}

    void to_json(nlohmann::json& nlohmann_json_j) const
    {
        to_json(nlohmann_json_j, static_cast<const TransferStrategyOptions&>(*this));
    }
    void from_json(const nlohmann::json& nlohmann_json_j)
    {
        to_json(nlohmann_json_j, static_cast<TransferStrategyOptions&>(*this));
    }
};
struct SingleAttributeTransferStrategyParameters
{
    wmtk::components::multimesh::utils::AttributeDescription attribute;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantParameters)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(SingleAttributeTransferStrategyParameters, attribute);
};

} // namespace wmtk::components::configurator::transfer
