#include "IsotropicRemeshingOptions.hpp"
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <fmt/format.h>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/Mesh.hpp>

#define DEFAULT_PARSABLE_ARGS                                                             \
    iterations, lock_boundary, use_for_periodic, fix_uv_seam, intermediate_output_format, \
        use_split, use_swap, use_collapse, use_smooth

namespace wmtk::components::isotropic_remeshing {
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

double IsotropicRemeshingOptions::get_absolute_length() const
{
    double length = length_abs;
    if (length_abs <= 0) {
        if (length_rel <= 0) {
            throw std::runtime_error("Either absolute or relative length must be set!");
        }
        length = relative_to_absolute_length(position_attribute, length_rel);
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
    {
        std::string name;
        switch (nlohmann_json_t.edge_swap_mode) {
        case EdgeSwapMode::AMIPS: name = "amips"; break;
        case EdgeSwapMode::Valence: name = "valence"; break;
        default:
        case EdgeSwapMode::Skip: name = "skip"; break;
        }
        nlohmann_json_j["edge_swap_mode"] = name;
    }

    if (!nlohmann_json_t.intermediate_output_format.empty()) {
        NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, intermediate_output_format));
    }
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

    assert(nlohmann_json_j.contains("edge_swap_mode"));
    const std::string swap_name = nlohmann_json_j["edge_swap_mode"].get<std::string>();
    if (swap_name == "amips") {
        nlohmann_json_t.edge_swap_mode = EdgeSwapMode::AMIPS;
    } else if (swap_name == "valence") {
        nlohmann_json_t.edge_swap_mode = EdgeSwapMode::Valence;
    } else if (swap_name == "skip") {
        nlohmann_json_t.edge_swap_mode = EdgeSwapMode::Skip;
    } else {
        throw std::runtime_error(fmt::format(
            "Expected edge_swap_mode to be one of [amips,valence,skip], got [{}]",
            swap_name));
    }
}
void IsotropicRemeshingOptions::load_json(const nlohmann::json& j, wmtk::components::multimesh::MeshCollection& mc)
{
    mesh_collection = &mc;
    from_json(j, *this);

    position_attribute =
        wmtk::components::multimesh::utils::get_attribute(mc, j["position_attribute"]);

    assert(position_attribute.is_valid());

    if (j.contains("inversion_position_attribute")) {
        inversion_position_attribute = wmtk::components::multimesh::utils::get_attribute(
            mc,
            j["inversion_position_attribute"]);
    }
    if (j.contains("other_position_attributes")) {
        for (const auto& other : j["other_position_attributes"]) {
            other_position_attributes.emplace_back(
                wmtk::components::multimesh::utils::get_attribute(mc, other));
        }
    }
    if (j.contains("pass_through_attributes")) {
        for (const auto& other : j["pass_through_attributes"]) {
            pass_through_attributes.emplace_back(
                wmtk::components::multimesh::utils::get_attribute(mc, other));
            assert(pass_through_attributes.back().is_valid());
        }
    }
    if (j.contains("copied_attributes")) {
        for (const auto& [child,parent]: j["copied_attributes"].items()) {
            copied_attributes.emplace_back(
                wmtk::components::multimesh::utils::get_attribute(mc, wmtk::components::multimesh::utils::AttributeDescription(child,std::optional<int64_t>{},{})),
                wmtk::components::multimesh::utils::get_attribute(mc, parent)
                );
            assert(copied_attributes.back().first.is_valid());
            assert(copied_attributes.back().second.is_valid());
        }
    }
    for (const auto& attr : pass_through_attributes) {
        spdlog::info("Pass through: {}", attr.name());
    }
    if (j.contains("static_mesh_names")) {
        for (const auto& other : j["static_mesh_names"]) {
            static_mesh_names.emplace_back(other);
        }
    }

    if (j.contains("intermediate_output_format")) {
    spdlog::info("making intermediate opts");
    intermediate_output_format = j["intermediate_output_format"];
    spdlog::info("end making intermediate opts");
    }
    assert(position_attribute.is_valid());
}
void IsotropicRemeshingOptions::write_json(nlohmann::json& js) const
{
    to_json(js, *this);
}

std::vector<wmtk::attribute::MeshAttributeHandle> IsotropicRemeshingOptions::all_positions() const
{
    std::vector<wmtk::attribute::MeshAttributeHandle> r = other_position_attributes;
    r.emplace_back(position_attribute);
    if (inversion_position_attribute.has_value()) {
        r.emplace_back(inversion_position_attribute.value());
    }
    std::sort(r.begin(), r.end());

    r.erase(std::unique(r.begin(), r.end()), r.end());

    return r;
}

} // namespace wmtk::components::isotropic_remeshing
