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

// An attribute that only de
struct MeshInvariantOptions : public InvariantOptions
{
    struct Parameters
    {
        std::string mesh_path;
        bool on_every_mesh;
        WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Parameters)
    };
    Parameters get_parameters() const;
    void set_parameters(const Parameters& p) const;
    std::string mesh_path() const;
    bool on_every_mesh() const;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MeshInvariantOptions)
    // void to_json(nlohmann::json& j) const override;
    MeshInvariantOptions(const InvariantOptions& o)
        : InvariantOptions(o)
    {}
    MeshInvariantOptions(std::string_view type, std::string_view name);
    MeshInvariantOptions(
        std::string_view type,
        const components::multimesh::MeshCollection& mc,
        const Mesh& mesh);
    MeshInvariantOptions(const MeshInvariantOptions& opts);
    MeshInvariantOptions(MeshInvariantOptions&& opts);
    MeshInvariantOptions operator=(const MeshInvariantOptions& opts);
    MeshInvariantOptions operator=(MeshInvariantOptions&& opts);
    MeshInvariantOptions(const InvariantOptions& o);
};


struct AttributeInvariantOptions : public InvariantOptions
{
    struct Parameters
    {
        wmtk::components::multimesh::utils::AttributeDescription attribute;
        WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Parameters)
    };
    Parameters get_parameters() const;
    void set_parameters(const Parameters& p) const;
    AttributeInvariantOptions(
        std::string_view type,
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
