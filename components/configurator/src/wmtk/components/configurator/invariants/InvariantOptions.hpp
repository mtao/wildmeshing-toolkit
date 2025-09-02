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

    InvariantOptions(const std::string& type, const nlohmann::json& params = {});
    //template <typename T>
    //InvariantOptions(const std::string& type, const T& params): InvariantOptions(type,nlohmann::json(params)) {}

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
struct AttributeInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription attribute;
    // WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantParameters)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(AttributeInvariantParameters, attribute);
};
struct EnvelopeInvariantOptions : public AttributeInvariantParameters
{
    double size;
    //    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(EnvelopeInvariantOptions, attribute, size);
};
struct InvariantCollectionParameters : public MeshInvariantParameters
{
    std::map<std::string, InvariantOptions> invariants;
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
};

/*
// An attribute that only de
struct MeshInvariantOptions : public TypedInvariantOptions<MeshInvariantParameters>
{
    using ParameterType = MeshInvariantParameters;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantOptions)
    std::string mesh_path() const;
    bool on_every_mesh() const;
    using Base = TypedInvariantOptions<MeshInvariantParameters>;
    using Base::Base;
    using Base::operator=;
};


struct AttributeInvariantOptions : public TypedInvariantOptions<AttributeInvariantParameters>
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(AttributeInvariantOptions)
    AttributeInvariantOptions(
        std::string_view type = {},
        const wmtk::components::multimesh::utils::AttributeDescription& = {});
    using Base = TypedInvariantOptions<AttributeInvariantParameters>;
    using Base::Base;
    using Base::operator=;

    wmtk::components::multimesh::utils::AttributeDescription attribute() const;
    void set_attribute(const wmtk::components::multimesh::utils::AttributeDescription&);
    // void to_json(nlohmann::json& j) const override;
};
struct EnvelopeInvariantOptions: public  TypedInvariantOptions<EnvelopeInvariantOptions>
{
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
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
*/
} // namespace wmtk::components::configurator::invariants
