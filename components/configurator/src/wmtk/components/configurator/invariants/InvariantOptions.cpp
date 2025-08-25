
#include "InvariantOptions.hpp"
#include <nlohmann/json.hpp>
namespace wmtk::components::configurator::invariants {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(InvariantOptions){
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type, parameters)


    // nlohmann_json_t.parameters->to_json(nlohmann_json_j["parameters"]);

    //
} InvariantOptions::~InvariantOptions() = default;
InvariantOptions::InvariantOptions() = default;
InvariantOptions::InvariantOptions(const InvariantOptions& o) = default;
InvariantOptions::InvariantOptions(InvariantOptions&&) = default;
InvariantOptions& InvariantOptions::operator=(const InvariantOptions&) = default;
InvariantOptions& InvariantOptions::operator=(InvariantOptions&&) = default;

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(InvariantOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(type, parameters)
    // auto& p = nlohmann_json_t.parameters;
    // if (nlohmann_json_t.type == "envelope") {
    //     p = std::make_unique<EnvelopeInvariantOptions>(
    //         nlohmann_json_j.get<EnvelopeInvariantOptions>());
    // } else if (nlohmann_json_t.type == "collection") {
    //     p = std::make_unique<InvariantCollectionOptions>(
    //         nlohmann_json_j.get<InvariantCollectionOptions>());
    // } else {
    //     p = std::make_unique<AttributeInvariantOptions>(
    //         nlohmann_json_j.get<AttributeInvariantOptions>());
    // }
}

// InvariantOptions::InvariantOptions() = default;
// InvariantOptions::~InvariantOptions() = default;
// WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(InvariantOptions)
//{}
//
// WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(InvariantOptions) {}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(MeshInvariantOptions)
{
    //

    to_json(nlohmann_json_j, static_cast<const InvariantOptions&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(MeshInvariantOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(MeshInvariantOptions);

    from_json(nlohmann_json_j, static_cast<InvariantOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(AttributeInvariantOptions)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(attribute)
    to_json(nlohmann_json_j, static_cast<const InvariantOptions&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(AttributeInvariantOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(attribute)

    from_json(nlohmann_json_j, static_cast<InvariantOptions&>(nlohmann_json_t));
}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(AliasInvariantOptions)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(invariant_name)
    to_json(nlohmann_json_j, static_cast<const InvariantOptions&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(AliasInvariantOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(invariant_name)

    from_json(nlohmann_json_j, static_cast<InvariantOptions&>(nlohmann_json_t));
} // void AttributeInvariantOptions::to_json(nlohmann::json& j) const
//{
//    invariants::to_json(j, *this);
//}
// void InvariantCollectionOptions::to_json(nlohmann::json& j) const
//{
//    invariants::to_json(j, *this);
//}
// void EnvelopeInvariantOptions::to_json(nlohmann::json& j) const
//{
//    invariants::to_json(j, *this);
//}
// void AliasInvariantOptions::to_json(nlohmann::json& j) const
//{
//    invariants::to_json(j, *this);
//}
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(InvariantCollectionOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(invariants)
    to_json(nlohmann_json_j, static_cast<const InvariantOptions&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(InvariantCollectionOptions)
{
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(invariants)

    from_json(nlohmann_json_j, static_cast<InvariantOptions&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(EnvelopeInvariantOptions)
{
    //
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(size)
    to_json(nlohmann_json_j, static_cast<const AttributeInvariantOptions&>(nlohmann_json_t));
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(EnvelopeInvariantOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(size)
    // from_json(nlohmann_json_j, static_cast<AttributeInvariantOptions&>(nlohmann_json_t));
}

AttributeInvariantOptions::AttributeInvariantOptions(
    std::string_view type,
    const wmtk::components::multimesh::utils::AttributeDescription&);

AttributeInvariantOptions::AttributeInvariantOptions(const AttributeInvariantOptions& opts) =
    default;
AttributeInvariantOptions::AttributeInvariantOptions(AttributeInvariantOptions&& opts) = default;
AttributeInvariantOptions AttributeInvariantOptions::operator=(
    const AttributeInvariantOptions& opts) = default;
AttributeInvariantOptions AttributeInvariantOptions::operator=(AttributeInvariantOptions&& opts) =
    default;
// AttributeInvariantOptions::AttributeInvariantOptions(
//     const wmtk::components::multimesh::utils::AttributeDescription&);
} // namespace wmtk::components::configurator::invariants
