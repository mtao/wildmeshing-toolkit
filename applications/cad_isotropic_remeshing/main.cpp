
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/applications/utils/element_count_report.hpp>
#include <wmtk/applications/utils/get_integration_test_data_root.hpp>
#include <wmtk/applications/utils/parse_jse.hpp>
#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/MultimeshOptions.hpp>
#include <wmtk/components/multimesh/multimesh.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/multimesh/same_simplex_dimension_bijection.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>
#include <wmtk/utils/cast_attribute.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>
#include "wmtk/components/utils/PathResolver.hpp"

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/components/input/input.hpp>
#include <wmtk/components/isotropic_remeshing/IsotropicRemeshingOptions.hpp>
#include <wmtk/components/isotropic_remeshing/isotropic_remeshing.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/utils/resolve_path.hpp>
#include <wmtk/components/multimesh/from_manifold_decomposition.hpp>

#include <h5pp/h5pp.h>
#include <wmtk/components/output/output.hpp>
#include "EigenMeshes.hpp"
#include "Mesh.hpp"

using namespace wmtk::components;
using namespace wmtk;
namespace fs = std::filesystem;

constexpr static std::string root_attribute_name = "root";

int main(int argc, char* argv[])
{
    h5pp::File file(argv[1], h5pp::FileAccess::READONLY);

    FusedOutput fo;
    fo.load(file, "/");
    auto trimesh = fo.mesh.create();


       auto edge_MD = wmtk::components::multimesh::from_manifold_decomposition(*trimesh);
    //kstd::vector<std::array<std::shared_ptr<wmtk::Mesh>,2>> manifold_decomposition = wmtk::components::multimesh::from_manifold_decomposition(*trimesh);

    /*
    auto& F = fo.mesh.F;
    F = std::get<0>(wmtk::utils::internal::boundary_manifold_decomposition<3>(F));
    auto manifold = std::make_shared<wmtk::TriMesh>();
    manifold->initialize(F);
    auto map = wmtk::multimesh::same_simplex_dimension_bijection(*trimesh, *manifold);

    trimesh->register_child_mesh(manifold, map);

    auto mah = fo.topology.add_feature_edge_mesh_tag(*trimesh, "edge_tag");

    auto pos = trimesh->get_attribute_handle<double>("vertices", wmtk::PrimitiveType::Vertex);
    auto edge_tag = wmtk::utils::cast_attribute<char>(mah, *manifold, "edge_tag");
    pos = wmtk::utils::cast_attribute<char>(pos, *manifold, "vertices");
    {
        auto p = std::dynamic_pointer_cast<wmtk::EdgeMesh>(
            wmtk::components::multimesh::from_tag(edge_tag, 1, {pos}));
        // p->delete_attribute(p->get_attribute_handle<char>("edge_tag",
        // wmtk::PrimitiveType::Edge));
        manifold->delete_attribute(edge_tag);

        trimesh->deregister_child_mesh(manifold);
        trimesh = manifold;
    }
    */


    /*
    if (true) {
        auto em = fo.topology.feature_edge_mesh(*trimesh);
        // wmtk::multimesh::utils::check_maps_valid(*em);
        // wmtk::multimesh::utils::check_maps_valid(*trimesh);
        // return 0;
    }
    if (true) {
        auto pm = fo.topology.corner_mesh(*trimesh);
        // wmtk::multimesh::utils::check_maps_valid(*pm);
        // wmtk::multimesh::utils::check_maps_valid(*trimesh);
    }
    */


    wmtk::components::output::OutputOptions opts;
    opts.path = argv[2];
    opts.type = ".hdf5";
    // opts.position_attribute =
    // trimesh->get_attribute_handle<double>("vertices",wmtk::PrimitiveType::Vertex);
    spdlog::info("end making intermediate opts");
    wmtk::components::output::output(*trimesh, opts);

    // auto out_opts = j["output"].get<wmtk::components::output::OutputOptions>();
    // out_opts.position_attribute = options.position_attribute;
    // wmtk::components::output::output(*mesh_ptr, out_opts);


    // for(const auto&
    /*
std::filesystem::path p = "/meshes";
auto groups = file.findGroups("","/meshes",-1,0);
spdlog::info("{}", groups.size());
for(const auto& g: groups) {
    spdlog::info("{}", g);
    FusedOutput fo;
    fo.load(file, p/g);
}
*/
}
/*
int main(int argc, char* argv[])
{
    CLI::App app{argv[0]};

    app.ignore_case();

    fs::path json_input_file;
    fs::path json_integration_config_file;
    app.add_option("-j, --json", json_input_file, "json specification file")
        ->required(true)
        ->check(CLI::ExistingFile);
    app.add_option(
           "-i, --integration-test-config",
           json_integration_config_file,
           "Test config file for integration test")
        ->check(CLI::ExistingFile);
    CLI11_PARSE(app, argc, argv);

    // nlohmann::json j = wmtk::applications::utils::parse_jse(
    //     wmtk::applications::isotropic_remeshing::spec,
    //     json_input_file);

    spdlog::warn("File is {}", json_input_file.string());
    std::ifstream ifs(json_input_file);
    nlohmann::ordered_json j = nlohmann::json::parse(ifs);

    const auto& input_js = j["input"];
    components::utils::PathResolver path_resolver;

    if (j.contains(root_attribute_name)) {
        path_resolver = j[root_attribute_name];
    }
    if (!json_integration_config_file.empty()) {
        auto path = wmtk::applications::utils::get_integration_test_data_root(
            json_integration_config_file,
            argv[0]);
        path_resolver.add_path(path);
    }

    wmtk::components::multimesh::MeshCollection mc;

    auto add = [&](const auto& my_input_js) {
        auto input_opts = my_input_js.template get<wmtk::components::input::InputOptions>();
        auto& named_mesh = mc.add_mesh(wmtk::components::input::input(input_opts, path_resolver));

        if (my_input_js.contains("multimesh")) {
            const nlohmann::ordered_json mm_js = my_input_js["multimesh"];
            if (mm_js.is_array()) {
                for (const auto& single_mm : mm_js) {
                    wmtk::components::multimesh::multimesh(mc, single_mm);
                }
            } else {
                wmtk::components::multimesh::multimesh(mc, mm_js);
            }
        }
    };
    if (input_js.is_array()) {
        for (const auto& js : input_js) {
            add(js);
        }
    } else {
        add(input_js);
    }


    if (!mc.is_valid()) {
        wmtk::logger().error("Input mesh did not match the name specification, going to throw an "
                             "exception to help debugging");
        mc.is_valid(true);
    }
    spdlog::info("Parsing isotropic params");

    wmtk::components::isotropic_remeshing::IsotropicRemeshingOptions options;

    options.load_json(j, mc);
    spdlog::info("filling in mesh attributes");
    if (input_js.contains("improvement_attributes")) {
        for (const auto& attribute : input_js["improvement_attributes"]) {
            options.improvement_attributes.emplace_back(
                wmtk::components::multimesh::utils::get_attribute(mc, attribute));
        }
    }


    wmtk::components::isotropic_remeshing::isotropic_remeshing(options);

    // input uv mesh

    // multimesh the input meshes if not already multimeshed
    // OR - should this be a diff app?

    // call isotropic_remeshing


    spdlog::info("making opts");
    auto output_opts = j["output"].get<wmtk::components::output::OutputOptionsCollection>();
    spdlog::info("end making intermediate opts");
    wmtk::components::output::output(mc, output_opts);

    // auto out_opts = j["output"].get<wmtk::components::output::OutputOptions>();
    // out_opts.position_attribute = options.position_attribute;
    // wmtk::components::output::output(*mesh_ptr, out_opts);

    if (j.contains("report")) {
        const std::string report = j["report"];
        mc.make_canonical();
        if (!report.empty()) {
            nlohmann::json out_json;
            auto& stats = out_json["stats"];
            stats = wmtk::applications::utils::element_count_report_named(mc);
            j.erase("report");
            if (j.contains(root_attribute_name)) {
                j.erase(root_attribute_name);
            }
            out_json["input"] = j;


            std::ofstream ofs(report);
            ofs << std::setw(2) << out_json;
        }
    }
}
*/
