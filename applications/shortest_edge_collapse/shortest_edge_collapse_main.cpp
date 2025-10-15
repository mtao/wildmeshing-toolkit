#include <jse/jse.h>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <wmtk/applications/utils/element_count_report.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/applications/utils/read_inputs.hpp>
#include <wmtk/components/input/input.hpp>
#include <wmtk/components/multimesh/multimesh.hpp>
#include <wmtk/components/multimesh/utils/get_attribute_description.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/components/shortest_edge_collapse/shortest_edge_collapse.hpp>
#include <wmtk/components/utils/resolve_path.hpp>

#include "shortest_edge_collapse_spec.hpp"

using namespace wmtk;
namespace fs = std::filesystem;


using wmtk::components::utils::resolve_paths;

namespace {

enum class MultiMeshOptions { None, OptBoundary, OptInterior };

NLOHMANN_JSON_SERIALIZE_ENUM(
    MultiMeshOptions,
    {{MultiMeshOptions::None, "none"},
     {MultiMeshOptions::OptInterior, "interior"},
     {MultiMeshOptions::OptBoundary, "boundary"}});

} // namespace

int main(int argc, char* argv[])
{
    CLI::App app{argv[0]};

    app.ignore_case();

    fs::path json_input_file;
    app.add_option("-j, --json", json_input_file, "json specification file")
        ->required(true)
        ->check(CLI::ExistingFile);
    CLI11_PARSE(app, argc, argv);

    nlohmann::json j;
    {
        std::ifstream ifs(json_input_file);
        j = nlohmann::json::parse(ifs);

        jse::JSE spec_engine;
        bool r = spec_engine.verify_json(j, shortest_edge_collapse_spec);
        if (!r) {
            wmtk::logger().error("{}", spec_engine.log2str());
            return 1;
        } else {
            j = spec_engine.inject_defaults(j, shortest_edge_collapse_spec);
        }
    }

    // const fs::path input_file = resolve_paths(json_input_file, {j["input_path"], j["input"]});

    std::vector<std::filesystem::path> additional_paths;
    if (j.contains("input_path")) {
        additional_paths.emplace_back(j["input_path"]);
    }
    if (j.contains("input_path")) {
        additional_paths.emplace_back(j["input"]);
    }

    wmtk::components::multimesh::MeshCollection mc =
        wmtk::applications::utils::read_inputs(j, "input", "root", additional_paths);

    attribute::MeshAttributeHandle pos_handle = wmtk::components::multimesh::utils::get_attribute(
        mc,
        components::multimesh::utils::AttributeDescription{
            "/vertices",
            0, // vertex
            attribute::AttributeType::Double});
    std::shared_ptr<Mesh> mesh_in = pos_handle.mesh().shared_from_this();

    attribute::MeshAttributeHandle other_pos_handle;

    // create multi-mesh
    std::shared_ptr<Mesh> current_mesh = mesh_in;
    std::shared_ptr<Mesh> other_mesh;
    MultiMeshOptions mm_opt = j["use_multimesh"];

    if (mm_opt != MultiMeshOptions::None) {
        Mesh& mesh = mc.get_mesh("");

        auto [parent_mesh, child_mesh] = wmtk::components::multimesh::multimesh(
            wmtk::components::multimesh::MultiMeshType::Boundary,
            mesh,
            nullptr,
            pos_handle,
            "",
            -1,
            -1);
        parent_mesh->clear_attributes({pos_handle});

        if (mm_opt == MultiMeshOptions::OptBoundary) {
            current_mesh = child_mesh;
            other_mesh = parent_mesh;
        } else {
            current_mesh = parent_mesh;
            other_mesh = child_mesh;
        }
        pos_handle = current_mesh->get_attribute_handle<double>("vertices", PrimitiveType::Vertex);
        other_pos_handle =
            other_mesh->get_attribute_handle<double>("vertices", PrimitiveType::Vertex);
    }


    // shortest-edge collapse
    {
        using namespace components::shortest_edge_collapse;
        ShortestEdgeCollapseOptions options;
        options.position_handle = pos_handle;
        if (other_mesh) {
            options.other_position_handles.emplace_back(other_pos_handle);
        }

        options.length_rel = j["length_rel"];
        const double env_size = j["envelope_size"];
        if (env_size >= 0) {
            options.envelope_size = j["envelope_size"];
        }
        options.lock_boundary = j["lock_boundary"];
        options.check_inversions = j["check_inversion"];

        shortest_edge_collapse(mc, options);
    }


    components::output::OutputOptionsCollection out_opts;
    if (j["output"].is_string()) {
        wmtk::logger().info(
            "For historic purposes output options as a single path will result in many meshes "
            "being output");

        components::output::OutputOptions opts;
        opts.path = j["output"].get<std::filesystem::path>();
        opts.position_attribute = {"/vertices"};
        opts.type = ".vtu";
        out_opts.emplace_back("", opts);

        auto& mesh = *mesh_in;
        // output child meshes
        {
            const std::string output_name = j["output"];
            const auto children = mesh.get_all_child_meshes();
            for (size_t i = 0; i < children.size(); ++i) {
                Mesh& child = *children[i];
                if (!child.has_attribute<double>("vertices", PrimitiveType::Vertex)) {
                    logger().warn("Child has no vertices attribute");
                    continue;
                }
                auto ph = child.get_attribute_handle<double>("vertices", PrimitiveType::Vertex);
                wmtk::components::output::output(
                    child,
                    fmt::format("{}_child_{}", output_name, i),
                    ph);
                opts.path = fmt::format("{}_child_{}", output_name, i);
                opts.position_attribute =
                    components::multimesh::utils::get_attribute_description(mc, ph);
                out_opts.emplace_back("", opts);
            }
        }
    } else {
        out_opts = components::output::parse_output(j["output"]);
    }
    wmtk::components::output::output(mc, out_opts);
    // wmtk::components::output::output(mc, j["output"], pos_handle);


    const std::string report = j["report"];
    if (!report.empty()) {
        Mesh& mesh = mc.get_mesh("");
        nlohmann::json out_json;

        auto& stats = out_json["stats"];
        stats = wmtk::applications::utils::element_count_report_named(mc);

        out_json["input"] = j;

        std::ofstream ofs(report);
        ofs << std::setw(4) << out_json;
    }


    return 0;
}
