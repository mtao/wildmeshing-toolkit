
#include "InvariantOptions.hpp"
#include <nlohmann/json.hpp>
namespace wmtk::components::configurator::invariants {
InvariantOptions::~InvariantOptions() = default;
InvariantOptions::InvariantOptions() = default;
InvariantOptions::InvariantOptions(const InvariantOptions& o) = default;
InvariantOptions::InvariantOptions(InvariantOptions&&) = default;
InvariantOptions& InvariantOptions::operator=(const InvariantOptions&) = default;
InvariantOptions& InvariantOptions::operator=(InvariantOptions&&) = default;
InvariantOptions::InvariantOptions(const std::string& t, const nlohmann::json& params)
    : type(t)
    , parameters(params)
{}

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InvariantOptions, type, parameters);


// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MeshInvariantParameters, mesh_path, on_every_mesh);
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AttributeInvariantParameters, attribute);

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(EnvelopeInvariantOptions, attribute, size);
//  NLOHMANN_DEFINE_DERIVED_TYPE_NON_INTRUSIVE_WITH_DEFAULT(
//      EnvelopeInvariantOptions,
//      AttributeInvariantParameters,
//      size);

// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(InvariantCollectionParameters, invariants);
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(AliasInvariantParameters, name);
} // namespace wmtk::components::configurator::invariants
