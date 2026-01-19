#include "IsotropicRemeshingOptions.hpp"
#include <fmt/format.h>
#include <Eigen/Dense>
#include <Eigen/Geometry>
#include <algorithm>
#include <nlohmann/json.hpp>
#include <wmtk/components/configurator/transfer/TransferStrategyFactory.hpp>
#include <wmtk/components/configurator/transfer/init.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/utils/Logger.hpp>
#include "wmtk/components/configurator/invariants/InvariantFactory_default_functors.hpp"


#include <wmtk/Mesh.hpp>
#include "wmtk/components/configurator/PassConfiguration.hpp"
#include "wmtk/components/configurator/invariants/InvariantOptions.hpp"
#include "wmtk/components/configurator/transfer/TransferStrategyFactoryRegistry.hpp"

#define DEFAULT_PARSABLE_ARGS                                                           \
    lock_boundary, intermediate_output_format, start_with_collapse, position_attribute, \
        pass_through_attributes, static_meshes, improvement_attributes, utility_attributes


namespace wmtk::components::isotropic_remeshing {

IsotropicRemeshingOptions::IsotropicRemeshingOptions()
    : IsotropicRemeshingOptions(multimesh::utils::AttributeDescription("/vertices"))
{}

IsotropicRemeshingOptions::IsotropicRemeshingOptions(
    const multimesh::utils::AttributeDescription& position_attr)
{
    wmtk::components::configurator::transfer::init();

    position_attribute = position_attr;

    const std::string_view mesh_path = position_attribute.mesh_path();
    operations["split"] = configurator::operations::EdgeSplitOptions{mesh_path};
    operations["collapse"] = configurator::operations::EdgeCollapseOptions{mesh_path};
    operations["collapse"].add_alias_invariant("link_condition");
    operations["collapse"].add_alias_invariant("multimesh_valid_map");
    operations["smooth"] =
        configurator::operations::AttributeUpdateOptions(position_attribute, "vertex_smooth");
    // operations["swap"].add_alias_invariant("interior_simplex");

    {
        auto so = configurator::operations::EdgeSwapOptions{mesh_path};
        auto sp = so.get_parameters();
        so.add_alias_invariant("valence_improvement");
        // so.add_alias_invariant("interior_simplex");
        // so.add_alias_invariant("cannot_map");
        //  sp.collapse_invariants["link_condition"] =
        //      operations["collapse"].invariants["link_condition"];

        sp.collapse_invariants["multimesh_valid_map"] =
            operations["collapse"].invariants["multimesh_valid_map"];
        so.set_parameters(sp);
        operations["swap"] = so;
    }
    passes = {configurator::PassOptions{
        .mesh_path = std::string(mesh_path),
        .operations = {{"split"}, {"collapse"}, {"swap"}, {"smooth"}},.default_scheduler = {.mesh_path = std::string(mesh_path)}}};
}

void IsotropicRemeshingOptions::process_custom_options()
{
    if (separate_substructures) {
        operations["collapse"].add_alias_invariant("separate_substructures");
    }
    if (!static_meshes.empty()) {
        configurator::invariants::InvariantCollectionParameters smp;
        smp.mesh_path = position_attribute.mesh_path();
        for (const auto& mesh_name : static_meshes) {
            // configurator::invariants::TypedInvariantOptions<
            //     configurator::invariants::CannotMapSimplexInvariantParameters>
            //     p;
            configurator::invariants::CannotMapSimplexInvariantParameters p;
            p.simplex_dimension = 0;
            p.mesh_path = position_attribute.mesh_path();
            p.mapped_mesh_path = mesh_name;
            std::string name = fmt::format(
                "cannot_map_{}-[{}]_to_[{}]",
                p.simplex_dimension,
                p.mesh_path,
                p.mapped_mesh_path);
            // auto [it, did] =
            smp.invariants.emplace_back(
                name,
                configurator::invariants::InvariantOptions("cannot_map", p));
        }
        invariants.try_emplace(
            "static_meshes",
            configurator::invariants::InvariantOptions("collection", smp));
        operations["collapse"].add_alias_invariant("static_meshes");
        operations["smooth"].add_alias_invariant("static_meshes");
    }
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
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(IsotropicRemeshingOptions)
{
    to_json(nlohmann_json_j, static_cast<const configurator::PassConfiguration&>(nlohmann_json_t));
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
        std::map<std::string, wmtk::components::multimesh::utils::AttributeDescription> ad;
        for (const auto& [target, src] : nlohmann_json_t.copied_attributes) {
            ad[target.path] = src;
        }
        nlohmann_json_j["copied_attributes"] = ad;
    }

    // if (nlohmann_json_t.envelope_size.has_value()) {
    //     nlohmann_json_j["envelope_size"] = nlohmann_json_t.envelope_size.value();
    // }


    // if (!nlohmann_json_t.intermediate_output_format.empty()) {
    //     NLOHMANN_JSON_EXPAND(NLOHMANN_JSON_PASTE(NLOHMANN_JSON_TO, intermediate_output_format));
    // }

    // nlohmann_json_j["utility_attributes"] = nlohmann_json_t.utility_attributes;
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(IsotropicRemeshingOptions)
{
    multimesh::utils::AttributeDescription pos_attr = nlohmann_json_j["position_attribute"];
    nlohmann_json_t = IsotropicRemeshingOptions(pos_attr);
    IsotropicRemeshingOptions nlohmann_json_default_obj = IsotropicRemeshingOptions(pos_attr);
    // WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(IsotropicRemeshingOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(DEFAULT_PARSABLE_ARGS);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(copied_attributes);

    std::map<std::string, multimesh::utils::AttributeDescription> copied_attributes;
    std::transform(
        nlohmann_json_t.copied_attributes.begin(),
        nlohmann_json_t.copied_attributes.end(),
        std::inserter(copied_attributes, copied_attributes.end()),
        [](const auto& pr) {
            return std::pair<const std::string, multimesh::utils::AttributeDescription>(
                pr.first.path,
                pr.second);
        });

    from_json(nlohmann_json_j, static_cast<configurator::PassConfiguration&>(nlohmann_json_t));

    // WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON(configurator);

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

wmtk::components::configurator::operations::EdgeSplitOptions IsotropicRemeshingOptions::get_split()
    const
{
    return operations.at("split");
}
wmtk::components::configurator::operations::EdgeCollapseOptions
IsotropicRemeshingOptions::get_collapse() const
{
    return operations.at("collapse");
}
wmtk::components::configurator::operations::AttributeUpdateOptions
IsotropicRemeshingOptions::get_smooth() const
{
    return operations.at("smooth");
}
wmtk::components::configurator::operations::EdgeSwapOptions IsotropicRemeshingOptions::get_swap()
    const
{
    return wmtk::components::configurator::operations::EdgeSwapOptions(operations.at("swap"));
}
void IsotropicRemeshingOptions::set_split(
    const wmtk::components::configurator::operations::EdgeSplitOptions& split)
{
    operations["split"] = split;
}
void IsotropicRemeshingOptions::set_collapse(
    const wmtk::components::configurator::operations::EdgeCollapseOptions& collapse)
{
    operations["collapse"] = collapse;
}
void IsotropicRemeshingOptions::set_smooth(
    const wmtk::components::configurator::operations::AttributeUpdateOptions& smooth)
{
    operations["smooth"] = smooth;
}
void IsotropicRemeshingOptions::set_swap(
    const wmtk::components::configurator::operations::EdgeSwapOptions& swap)
{
    operations["swap"] = swap;
}
} // namespace wmtk::components::isotropic_remeshing
