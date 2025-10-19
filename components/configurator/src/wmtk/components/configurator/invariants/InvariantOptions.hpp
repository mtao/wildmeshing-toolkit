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

    explicit InvariantOptions(const std::string& type, const nlohmann::json& params = {});
    // template <typename T>
    // InvariantOptions(const std::string& type, const T& params):
    // InvariantOptions(type,nlohmann::json(params)) {}

    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantOptions)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(InvariantOptions, type, parameters);
};


template <typename ParametersType>
struct TypedInvariantOptions : public InvariantOptions
{
    ParametersType get_parameters() const { return parameters.get<ParametersType>(); }
    void set_parameters(const ParametersType& p) const { parameters = p; }
    TypedInvariantOptions() = default;
    TypedInvariantOptions(const TypedInvariantOptions& opts) = default;
    TypedInvariantOptions(TypedInvariantOptions&& opts) = default;
    TypedInvariantOptions& operator=(const TypedInvariantOptions& opts) = default;
    TypedInvariantOptions& operator=(TypedInvariantOptions&& opts) = default;
    TypedInvariantOptions(const InvariantOptions& o)
        : InvariantOptions(o)
    {}

    void to_json(nlohmann::json& nlohmann_json_j) const
    {
        to_json(nlohmann_json_j, static_cast<const InvariantOptions&>(*this));
    }
    void from_json(const nlohmann::json& nlohmann_json_j)
    {
        to_json(nlohmann_json_j, static_cast<InvariantOptions&>(*this));
    }
};

struct MeshInvariantParameters
{
    std::string mesh_path;
    bool on_every_mesh = false;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantParameters)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(MeshInvariantParameters, mesh_path, on_every_mesh);
};
struct MeshSimplexInvariantParameters : public MeshInvariantParameters
{
    int8_t dimension;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantParameters)
    NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE(
        MeshSimplexInvariantParameters,
        MeshInvariantParameters,
        dimension);
};

struct AttributeInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription attribute;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantParameters)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AttributeInvariantParameters, attribute);
};

struct InvariantCollectionParameters : public MeshInvariantParameters
{
    std::vector<std::pair<std::string, InvariantOptions>> invariants;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(InvariantCollectionParameters)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(InvariantCollectionParameters, invariants, mesh_path);
};

// special invariant for referring to invariants from a cache
struct AliasInvariantParameters
{
    constexpr static std::string type_name = "alias";
    std::string name;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AliasInvariantParameters)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AliasInvariantParameters, name);
    wmtk::components::multimesh::utils::AttributeDescription threshold_attribute;
};


} // namespace wmtk::components::configurator::invariants
