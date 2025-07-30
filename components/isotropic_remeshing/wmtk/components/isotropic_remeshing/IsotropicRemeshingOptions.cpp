#include "IsotropicRemeshingOptions.hpp"
#include <fmt/format.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <wmtk/components/mesh_info/transfer/TransferStrategyFactory.hpp>
#include <wmtk/components/mesh_info/transfer/init.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/utils/Logger.hpp>


#include <wmtk/Mesh.hpp>
#include "wmtk/components/mesh_info/transfer/TransferStrategyFactoryRegistry.hpp"

#define DEFAULT_PARSABLE_ARGS                                                             \
    iterations, lock_boundary, intermediate_output_format, split, swap, collapse, smooth, \
        start_with_collapse, passes, position_attribute, copied_attributes,              \
        pass_through_attributes, static_meshes, improvement_attributes //, utility_attributes

namespace wmtk::components::isotropic_remeshing {

IsotropicRemeshingOptions::IsotropicRemeshingOptions()
{
    wmtk::components::mesh_info::transfer::init();
}
namespace {

// compute the length relative to the bounding box diagonal
double relative_to_absolute_length(
    const attribute::MeshAttributeHandle& position,
    const double length_rel)
{
    if (!position.is_valid()) {
        throw std::runtime_error(
            "Could not convert relative to absolute length because position attr was invalid");
    }
    auto pos = position.mesh().create_const_accessor<double>(position);
    const auto vertices = position.mesh().get_all(PrimitiveType::Vertex);
    Eigen::AlignedBox<double, Eigen::Dynamic> bbox(pos.dimension());


    for (const auto& v : vertices) {
        bbox.extend(pos.const_vector_attribute(v));
    }

    const double diag_length = bbox.sizes().norm();
    wmtk::logger().debug(
        "computed absolute target length using relative factor {} on bbox diagonal {}",
        length_rel,
        diag_length);

    return length_rel * diag_length;
}
} // namespace

double IsotropicRemeshingOptions::get_absolute_length(const multimesh::MeshCollection& mc) const
{
    double length = length_abs;
    if (length_abs <= 0) {
        if (length_rel <= 0) {
            throw std::runtime_error("Either absolute or relative length must be set!");
        }
        auto position_attr = multimesh::utils::get_attribute(mc, position_attribute);
        length = relative_to_absolute_length(position_attr, length_rel);
    } else {
        wmtk::logger().debug(
            "get_absolute_length using absolute length value {} {}",
            length,
            length_abs);
    }
    return length;
}
void to_json(nlohmann::json& nlohmann_json_j, const IsotropicRemeshingOptions& nlohmann_json_t)
{
    NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, DEFAULT_PARSABLE_ARGS));

    if (nlohmann_json_t.length_abs != 0) {
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, length_abs));
    } else {
        assert(nlohmann_json_t.length_rel != 0);
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, length_rel));
    }
    if (nlohmann_json_t.envelope_size.has_value()) {
        nlohmann_json_j["envelope_size"] = nlohmann_json_t.envelope_size.value();
    }

    if (nlohmann_json_t.envelope_size.has_value()) {
        nlohmann_json_j["envelope_size"] = nlohmann_json_t.envelope_size.value();
    }


    if (!nlohmann_json_t.intermediate_output_format.empty()) {
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, intermediate_output_format));
    }

    nlohmann_json_j["utility_attributes"] = nlohmann_json_t.utility_attributes;
}
void from_json(const nlohmann::json& nlohmann_json_j, IsotropicRemeshingOptions& nlohmann_json_t)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(IsotropicRemeshingOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(DEFAULT_PARSABLE_ARGS);

    if (nlohmann_json_j.contains("length_abs")) {
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, length_abs));
        wmtk::logger().debug("Got an absolute length {}", nlohmann_json_t.length_abs);
    } else {
        assert(nlohmann_json_j.contains("length_rel"));
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_FROM, length_rel));
        wmtk::logger().debug("Got a relative length {}", nlohmann_json_t.length_rel);
    }

    if (nlohmann_json_j.contains("envelope_size")) {
        nlohmann_json_t.envelope_size = nlohmann_json_j["envelope_size"].get<double>();
    }
    if (nlohmann_json_j.contains("utility_attributes")) {
        nlohmann_json_t.utility_attributes = nlohmann_json_j["utility_attributes"];
    }
}

std::vector<multimesh::utils::AttributeDescription> IsotropicRemeshingOptions::all_positions() const
{
    std::vector<multimesh::utils::AttributeDescription> r;
    // = other_position_attributes;
    r.emplace_back(position_attribute);
    /*
    if (inversion_position_attribute.has_value()) {
        r.emplace_back(inversion_position_attribute.value());
    }
    std::sort(r.begin(), r.end());

    r.erase(std::unique(r.begin(), r.end()), r.end());
    */

    return r;
}

void fill_operation_parameters() {}
} // namespace wmtk::components::isotropic_remeshing
