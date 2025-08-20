#pragma once
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/utils/json_macros.hpp>

namespace wmtk::components::configurator::invariants {
// struct InvariantOptions
//{
//     InvariantOptions();
//     virtual ~InvariantOptions();
//     WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
//     virtual void to_json(nlohmann::json& j) const = 0;
// };

struct InvariantOptions
{
    std::string type;
    // std::unique_ptr<InvariantOptions> parameters;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
};

// An attribute that only de
struct MeshInvariantOptions : public InvariantOptions
{
    std::string mesh_path;
    bool on_every_mesh = false;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
};


struct AttributeInvariantOptions : public InvariantOptions
{
    wmtk::components::multimesh::utils::AttributeDescription attribute;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
};
struct EnvelopeInvariantOptions : public AttributeInvariantOptions
{
    double size;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
};

struct InvariantCollectionOptions : public InvariantOptions
{
    std::vector<InvariantOptions> invariants;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantCollectionOptions)
    // void to_json(nlohmann::json& j) const override;
};

// special invariant for referring to invariants from a cache
struct AliasInvariantOptions : public InvariantOptions
{
    std::string invariant_name;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AliasInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
};
} // namespace wmtk::components::configurator::invariants
