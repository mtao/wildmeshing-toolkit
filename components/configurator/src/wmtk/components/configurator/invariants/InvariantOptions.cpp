
#include "InvariantOptions.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/bounding_box.hpp>
#include "wmtk/components/utils/get_attributes.hpp"
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

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(ThresholdInvariantParameters)
{
    to_json(nlohmann_json_j, static_cast<const AttributeInvariantParameters&>(nlohmann_json_t));
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type, parameters);
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(ThresholdInvariantParameters)
{
    from_json(nlohmann_json_j, static_cast<AttributeInvariantParameters&>(nlohmann_json_t));

    // if we are using bounding box but no threshold is set
    if (!nlohmann_json_j.contains("type")) {
        if (nlohmann_json_j.contains("relative_size")) {
            nlohmann_json_t.type = ThresholdInvariantParameters::ThresholdType::Absolute;
            BoundingBoxDiagonalThresholdParameters p;
            p.ratio = nlohmann_json_j["relative_size"];
            if (nlohmann_json_j.contains("threshold_attribute")) {
                logger().debug(
                    "Threshold invariant didn't have an envelope attribute, assuming it was input "
                    "mesh "
                    "{}",
                    nlohmann_json_t.attribute);
                p.threshold_attribute = nlohmann_json_t.attribute;
            }
            nlohmann_json_t.parameters = p;
        } else if (nlohmann_json_j.contains("absolute_size")) {
            nlohmann_json_t.type = ThresholdInvariantParameters::ThresholdType::Absolute;
            AbsoluteThresholdParameters p;
            p.threshold = nlohmann_json_j["size"];
            nlohmann_json_t.parameters = p;
        } else {
            wmtk::log_and_throw_error(
                "Threshold attribute did not have a type. Please specify a type or use "
                "\"relative_size\" or "
                "\"absolute_size\" to provide shortcut types (got [{}])",
                nlohmann_json_j.dump());
        }
        return;
    }
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(type, parameters);
}

double BoundingBoxDiagonalThresholdParameters::threshold(const multimesh::MeshCollection& mc) const
{
    auto attr = multimesh::utils::get_attribute(mc, threshold_attribute);

    return std::visit(
        [&](const auto& t) -> double {
            auto acc = attr.mesh().create_accessor(t);
            auto bb = wmtk::utils::bounding_box(acc);
            return bb.diagonal().template cast<double>().norm();
        },
        attr.handle());
}
double ThresholdInvariantParameters::threshold(const multimesh::MeshCollection& mc) const
{
    switch (type) {
    case ThresholdType::Absolute: {
        return AbsoluteThresholdParameters(parameters).threshold;
    }
    case ThresholdType::BoundingBoxDiagonalRelative: {
        return BoundingBoxDiagonalThresholdParameters(parameters).threshold(mc);
    }
    default: break;
    }
    return 0.0;
}

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
