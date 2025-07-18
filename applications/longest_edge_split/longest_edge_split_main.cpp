#include <jse/jse.h>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

//
#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/input/input.hpp>
#include <wmtk/components/multimesh/NamedMultiMesh.hpp>
#include <wmtk/components/utils/PathResolver.hpp>
//
#include <wmtk/components/longest_edge_split/longest_edge_split.hpp>
#include <wmtk/components/multimesh/multimesh.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/utils/resolve_path.hpp>

#include "longest_edge_split_spec.hpp"

using namespace wmtk;
namespace fs = std::filesystem;
constexpr static std::string root_attribute_name = "input_path";


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
        bool r = spec_engine.verify_json(j, longest_edge_split_spec);
        if (!r) {
            wmtk::logger().error("{}", spec_engine.log2str());
            return 1;
        } else {
            j = spec_engine.inject_defaults(j, longest_edge_split_spec);
        }
    }
    components::utils::PathResolver path_resolver;
    if (j.contains(root_attribute_name)) {
        path_resolver = j[root_attribute_name];
    }

    // const fs::path input_file = resolve_paths(json_input_file, {j["input_path"], j["input"]});

    std::shared_ptr<Mesh> mesh_in;

    for (const auto& p : {j["input_path"], j["input"]}) {
        wmtk::components::input::InputOptions input_opts;
        input_opts.path = p.get<std::string>();
        input_opts.ignore_z_if_zero = true;
        try {
            auto nmm = wmtk::components::input::input(input_opts, path_resolver);
            if (bool(nmm)) {
                mesh_in = nmm.root().shared_from_this();
                break;
            }
        } catch (const std::exception& e) {
        }
    }
    if (!mesh_in) {
        wmtk::logger().error("Could not find mesh");
        return 1;
    }
    // std::shared_ptr<Mesh> mesh_in = wmtk::components::input::input(input_file, true);

    attribute::MeshAttributeHandle pos_handle =
        mesh_in->get_attribute_handle<double>("vertices", PrimitiveType::Vertex);
    attribute::MeshAttributeHandle other_pos_handle;

    // create multi-mesh
    std::shared_ptr<Mesh> current_mesh = mesh_in;
    std::shared_ptr<Mesh> other_mesh;
    MultiMeshOptions mm_opt = j["use_multimesh"];

    if (mm_opt != MultiMeshOptions::None) {
        auto [parent_mesh, child_mesh] = wmtk::components::multimesh::multimesh(
            wmtk::components::multimesh::MultiMeshType::Boundary,
            *mesh_in,
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

    Mesh& mesh = *mesh_in;

    // longest edge split
    {
        using namespace components::longest_edge_split;
        LongestEdgeSplitOptions options;
        options.position_handle = pos_handle;
        if (other_mesh) {
            options.other_position_handles.emplace_back(other_pos_handle);
        }

        options.length_rel = j["length_rel"];

        longest_edge_split(mesh, options);
    }

    wmtk::components::output::output(mesh, j["output"], pos_handle);

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
            wmtk::components::output::output(child, fmt::format("{}_child_{}", output_name, i), ph);
        }
    }


    const std::string report = j["report"];
    if (!report.empty()) {
        nlohmann::json out_json;
        out_json["stats"]["vertices"] = mesh.get_all(PrimitiveType::Vertex).size();
        out_json["stats"]["edges"] = mesh.get_all(PrimitiveType::Edge).size();
        out_json["stats"]["triangles"] = mesh.get_all(PrimitiveType::Triangle).size();
        out_json["stats"]["tets"] = mesh.get_all(PrimitiveType::Tetrahedron).size();

        out_json["input"] = j;

        std::ofstream ofs(report);
        ofs << std::setw(4) << out_json;
    }


    return 0;
}
