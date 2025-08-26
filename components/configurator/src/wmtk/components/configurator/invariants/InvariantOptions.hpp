#pragma once
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/utils/json_macros.hpp>

namespace wmtk {
class Mesh;
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
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
    nlohmann::json parameters;

    InvariantOptions();
    InvariantOptions(const InvariantOptions&);
    InvariantOptions(InvariantOptions&&);
    InvariantOptions& operator=(const InvariantOptions&);
    InvariantOptions& operator=(InvariantOptions&&);
    ~InvariantOptions();
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
};


// template <typename ParametersType>
// struct TypedInvariantOptions: public InvariantOptions {
//     ParametersType get_parameters() const {
//         return parameters.get<ParametersType>();
//     }
//     void set_parameters(const ParametersType& p) const {
//         parameters = p;
//     }
//     TypedInvariantOptions(const TypedInvariantOptions& opts) = default;
//     TypedInvariantOptions(TypedInvariantOptions&& opts) = default;
//     TypedInvariantOptions operator=(const TypedInvariantOptions& opts) = default;
//     TypedInvariantOptions operator=(TypedInvariantOptions&& opts) = default;
//     TypedInvariantOptions(const InvariantOptions& o): InvariantOptions(o) {}
//
// };

struct MeshInvariantParameters
{
    std::string mesh_path;
    bool on_every_mesh;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantParameters)
};

// An attribute that only de
struct MeshInvariantOptions : public InvariantOptions
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantOptions)
    MeshInvariantParameters get_parameters() const;
    void set_parameters(const MeshInvariantParameters& p) const;
    std::string mesh_path() const;
    bool on_every_mesh() const;
    // void to_json(nlohmann::json& j) const override;
    MeshInvariantOptions(const InvariantOptions& o)
        : InvariantOptions(o)
    {}
    MeshInvariantOptions(std::string_view type = {}, std::string_view name = {});
    MeshInvariantOptions(
        std::string_view type,
        const components::multimesh::MeshCollection& mc,
        const Mesh& mesh);
    MeshInvariantOptions(const MeshInvariantOptions& opts);
    MeshInvariantOptions(MeshInvariantOptions&& opts);
    MeshInvariantOptions operator=(const MeshInvariantOptions& opts);
    MeshInvariantOptions operator=(MeshInvariantOptions&& opts);
};

struct AttributeInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription attribute;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantParameters)
};

struct AttributeInvariantOptions : public InvariantOptions
{
    AttributeInvariantParameters get_parameters() const;
    void set_parameters(const AttributeInvariantParameters& p) const;
    AttributeInvariantOptions(
        std::string_view type = {},
        const wmtk::components::multimesh::utils::AttributeDescription& = {});
    AttributeInvariantOptions(const AttributeInvariantOptions& opts);
    AttributeInvariantOptions(AttributeInvariantOptions&& opts);
    AttributeInvariantOptions operator=(const AttributeInvariantOptions& opts);
    AttributeInvariantOptions operator=(AttributeInvariantOptions&& opts);
    AttributeInvariantOptions(const InvariantOptions& o);

    wmtk::components::multimesh::utils::AttributeDescription attribute() const;
    void set_attribute(const wmtk::components::multimesh::utils::AttributeDescription&);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
};
struct EnvelopeInvariantOptions : public AttributeInvariantOptions
{
    double size;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
};

struct InvariantCollectionParameters : public InvariantOptions
{
    std::vector<InvariantOptions> invariants;
};

struct InvariantCollectionOptions : public InvariantOptions
{
    InvariantCollectionParameters get_parameters() const;
    void set_parameters(const InvariantCollectionParameters& p) const;
    InvariantCollectionOptions(std::string_view name = {});
    InvariantCollectionOptions(const InvariantCollectionOptions& opts);
    InvariantCollectionOptions(InvariantCollectionOptions&& opts);
    InvariantCollectionOptions operator=(const InvariantCollectionOptions& opts);
    InvariantCollectionOptions operator=(InvariantCollectionOptions&& opts);
    InvariantCollectionOptions(const InvariantOptions& o);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantCollectionOptions)
    // void to_json(nlohmann::json& j) const override;
};

// special invariant for referring to invariants from a cache
struct AliasInvariantParameters
{
    std::string name;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AliasInvariantParameters)
};
struct AliasInvariantOptions : public InvariantOptions
{
    AliasInvariantParameters get_parameters() const;
    void set_parameters(const AliasInvariantParameters& p) const;
    AliasInvariantOptions(std::string_view name = {});
    AliasInvariantOptions(const AliasInvariantOptions& opts);
    AliasInvariantOptions(AliasInvariantOptions&& opts);
    AliasInvariantOptions operator=(const AliasInvariantOptions& opts);
    AliasInvariantOptions operator=(AliasInvariantOptions&& opts);
    AliasInvariantOptions(const InvariantOptions& o);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AliasInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
    std::string get_name() const;
    void set_name(const std::string& name);
};
} // namespace wmtk::components::configurator::invariants
