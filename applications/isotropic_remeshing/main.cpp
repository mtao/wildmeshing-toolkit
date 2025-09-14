
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <wmtk/applications/utils/element_count_report.hpp>
#include <wmtk/applications/utils/get_integration_test_data_root.hpp>
#include <wmtk/applications/utils/parse_jse.hpp>
#include <wmtk/applications/utils/read_inputs.hpp>
#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/MultimeshOptions.hpp>
#include <wmtk/components/multimesh/multimesh.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/components/output/parse_output.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/components/input/input.hpp>
#include <wmtk/components/isotropic_remeshing/IsotropicRemeshingOptions.hpp>
#include <wmtk/components/isotropic_remeshing/isotropic_remeshing.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/utils/resolve_path.hpp>



using namespace wmtk::components;
using namespace wmtk;
namespace fs = std::filesystem;

constexpr static std::string root_attribute_name = "root";

int main(int argc, char* argv[])
{
    CLI::App app{argv[0]};

    app.ignore_case();

    fs::path json_input_file;
    fs::path log_file;
    fs::path json_integration_config_file;
    bool no_console_logging = false;
    app.add_option("-j, --json", json_input_file, "json specification file")
        ->required(true)
        ->check(CLI::ExistingFile);
    app.add_option("-l, --log_file", log_file, "log path");
    app.add_option(
        "--no-console-logging",
        no_console_logging,
        "whether the logger should output to the screen");
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


    // =====================
    // Parse input json file
    // =====================

    std::ifstream ifs(json_input_file);
    nlohmann::ordered_json j = nlohmann::ordered_json::parse(ifs);

    // =====================
    // Parse base settings
    // =====================
    if (!log_file.empty()) {
        wmtk::add_file_sink(log_file, /*keep_console_log \equiv*/ !no_console_logging);
    } else {
        if (no_console_logging) {
            logger().sinks().clear();
        }
    }


    // =====================
    // Parse input path util
    // =====================
    std::vector<std::filesystem::path> additional_paths;

    if (!json_integration_config_file.empty()) {
        auto path = wmtk::applications::utils::get_integration_test_data_root(
            json_integration_config_file,
            argv[0]);
        additional_paths.emplace_back(path);
    }
    additional_paths.emplace_back(json_input_file.parent_path());


    // =====================
    // Parse input path json
    // =====================

    wmtk::components::multimesh::MeshCollection mc =
        wmtk::applications::utils::read_inputs(j, "input", root_attribute_name, additional_paths);


    if (!mc.is_valid()) {
        wmtk::logger().error(
            "Input mesh did not match the name specification, going to throw an "
            "exception to help debugging");
        mc.is_valid(true);
    }
    spdlog::info("Parsing isotropic params");

    wmtk::components::isotropic_remeshing::IsotropicRemeshingOptions options = j;

    spdlog::info("filling in mesh attributes");
    if (j.contains("improvement_attributes")) {
        for (const auto& attribute : j["improvement_attributes"]) {
            options.improvement_attributes.emplace_back(
                wmtk::components::multimesh::utils::get_attribute(mc, attribute));
        }
    }

    spdlog::info("Multimesh structure: {}", mc.get_named_multimesh("").get_names_json()->dump(2));
    spdlog::info("Options: {}", nlohmann::json(options).dump(2));

    wmtk::components::isotropic_remeshing::isotropic_remeshing(mc, options);

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
