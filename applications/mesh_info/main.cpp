
#include <jse/jse.h>
#include <CLI/App.hpp>
#include <CLI/CLI.hpp>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>
#include <string>
#include <wmtk/applications/utils/element_count_report.hpp>
#include <wmtk/applications/utils/get_integration_test_data_root.hpp>
#include <wmtk/components/mesh_info/transfer/TransferStrategyFactoryCollection.hpp>

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/input/input.hpp>

#include <wmtk/components/output/OutputOptions.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/output/parse_output.hpp>
#include <wmtk/components/utils/resolve_path.hpp>

#include "CLI/CLI.hpp"
#include "wmtk/applications/utils/read_inputs.hpp"
#include "wmtk/components/multimesh/MeshCollection.hpp"
#include "wmtk/components/multimesh/utils/AttributeDescription.hpp"
#include "wmtk/components/multimesh/utils/get_attribute.hpp"
#include "wmtk/components/utils/PathResolver.hpp"

using namespace wmtk::components;
using namespace wmtk::applications;
using namespace wmtk;
namespace fs = std::filesystem;

namespace {


nlohmann::json compute(const attribute::MeshAttributeHandle& mah, std::string_view type)
{
    assert(mah.dimension() == 1);
    return std::visit(
        [&](const auto& attr) -> nlohmann::json {
            using T = std::decay_t<decltype(attr)>;
            using Scalar = typename T::Type;
            if constexpr (!std::is_same_v<Scalar, wmtk::Rational>) {
                auto acc = mah.mesh().create_const_accessor(attr);
                // std::vector<VectorX<Scalar>> values;
                std::vector<Scalar> values;
                for (const auto& t : mah.mesh().get_all(mah.primitive_type())) {
                    const Scalar v = acc.scalar_attribute(t);
                    values.emplace_back(v);
                }

                if (type == "mean") {
                    return std::accumulate(values.begin(), values.end(), Scalar(0)) / values.size();
                }
                if (type == "total") {
                    return std::accumulate(values.begin(), values.end(), Scalar(0));
                }
                std::sort(values.begin(), values.end());
                if (type == "min") {
                    return values.front();
                }
                if (type == "max") {
                    return values.back();
                }
                if (type == "median") {
                    return values[values.size() / 2];
                }
            }
            return {};
        },
        mah.handle());
}
} // namespace

int run_js(
    const std::string_view& app_name,
    const nlohmann::json& j,
    const std::optional<fs::path>& name_spec_file,
    const std::optional<fs::path>& integration_test_config_file)
{
    // if (name_spec_file.has_value()) {
    //     j["name"] = nlohmann::json::parse(std::ifstream(name_spec_file.value()));
    // }


    // if (!j.contains("output")) {
    //     std::cerr << "Turning off logger because we are printing to the screen";
    // wmtk::logger().set_level(spdlog::level::off);
    // }
    wmtk::components::multimesh::MeshCollection meshes;
    components::utils::PathResolver path_resolver;

    if (j.contains("root")) {
        path_resolver = j["root"];
    }
    if (integration_test_config_file.has_value()) {
        auto path = wmtk::applications::utils::get_integration_test_data_root(
            integration_test_config_file.value(),
            app_name);

        std::array<std::filesystem::path, 1> p{{path}};
        meshes = applications::utils::read_inputs(j, "input", "root", p);
    } else {
        meshes = applications::utils::read_inputs(j, "input", "root");
    }


    if (j.contains("utility_attributes")) {
        wmtk::components::mesh_info::transfer::TransferStrategyFactoryCollection transfers =
            j["utility_attributes"];
        for (const auto& transfer_ptr : transfers) {
            transfer_ptr->create(meshes, true);
        }
    }


    nlohmann::json out_json;
    auto& stats = out_json["element_counts"];
    stats = wmtk::applications::utils::element_count_report_named(meshes);

    if (j.contains("attributes")) {
        auto& out_attr_js = out_json["attributes"];
        for (const auto& [path, values_] : j["attributes"].items()) {
            auto attrs = wmtk::components::multimesh::utils::get_attributes(
                meshes,
                wmtk::components::multimesh::utils::AttributeDescription{path});

            std::string format = "{}";
            if (attrs.size() > 1) {
                format = "{}_{}";
            }
            std::vector<std::string> values;

            if (values_.is_string()) {
                // leave empty so "empty = catch everything" logic is easy
                if (values_ != "all") {
                    values.emplace_back(values_);
                }
            } else if (values_.is_array()) {
                values = values_.get<std::vector<std::string>>();
            }
            if (values.empty()) {
                values = {"min", "max", "median", "mean", "total"};
            }
            for (size_t k = 0; k < attrs.size(); ++k) {
                std::string output_tag = fmt::format(fmt::runtime(format), path, k);
                auto& this_attr = out_attr_js[output_tag];
                for (const auto& v : values) {
                    this_attr[v] = compute(attrs[k], v);
                }
            }
        }
    }

    if (j.contains("output")) {
        std::ofstream ofs(j["output"]);
        ofs << out_json;
    } else {
        std::cout << out_json.dump(2) << std::endl;
    }
    return 0;
}

int run(
    const std::string_view& app_name,
    const fs::path& config_path,
    const std::optional<fs::path>& name_spec_file,
    const std::optional<fs::path>& integration_test_config_file)
{
    nlohmann::json j;
    std::ifstream ifs(config_path);
    j = nlohmann::json::parse(ifs);

    return run_js(app_name, j, name_spec_file, integration_test_config_file);
}

int main(int argc, char* argv[])
{
    CLI::App app{argv[0]};

    app.ignore_case();

    fs::path json_input_file;
    std::optional<fs::path> json_integration_config_file;
    std::optional<std::string> json_integration_app_name;
    std::optional<fs::path> name_spec_file;

    auto add_it_path = [&](CLI::App& a) {
        a.add_option(
             "-c, --integration-test-config",
             json_integration_config_file,
             "Test config file for integration test")
            ->check(CLI::ExistingFile);
        a.add_option(
             "-a, --integration-test-app",
             json_integration_config_file,
             "Test config file for integration test")
            ->check(CLI::ExistingFile);
    };

    CLI::App* json_cmd = app.add_subcommand("json", "Run application using a json");
    json_cmd->add_option("-j, --json", json_input_file, "json specification file")
        ->required(true)
        ->check(CLI::ExistingFile);

    add_it_path(*json_cmd);


    fs::path input;
    fs::path output;
    std::string position = "vertices";
    std::string attributes;
    CLI::App* run_cmd = app.add_subcommand("run", "Convert mesh to another type");
    run_cmd->add_option("-i, --input", input, "input file")
        ->required(true)
        ->check(CLI::ExistingFile);

    run_cmd->add_option("--input-position", position, "input position attribute name");

    run_cmd->add_option("-o, --output", output, "output file");
    run_cmd->add_option("--attributes", attributes, "comma separated attributes");
    add_it_path(*run_cmd);


    CLI11_PARSE(app, argc, argv);

    // someday may add other suboptions

    // if (!json_input_file.has_value() && !fill_config_path.has_value()) {
    //     wmtk::logger().error("An input json file with [-j] is required unless blank config "
    //                          "generation is being used with [--fill-config]");
    //     return 1;
    // }

    int exit_mode = -1;

    // json_cmd->callback([&]() {
    //     spdlog::warn("YOW!");
    //     assert(json_input_file.has_value());
    //     exit_mode = run(json_input_file.value());
    // });
    if (json_cmd->parsed()) {
        exit_mode = run(argv[0], json_input_file, name_spec_file, json_integration_config_file);
    } else {
        wmtk::components::input::InputOptions in;
        in.path = input;

        nlohmann::json js;
        js["input"] = in;
        if (!output.empty()) {
            js["output"] = output;
        }

        std::vector<std::string> attrs;
        std::stringstream ss(attributes);
        std::string tok;
        while (std::getline(ss, tok, ',')) {
            attrs.emplace_back(tok);
        }
        auto& util_attr_js = js["utility_attributes"] = nlohmann::json::array();
        auto& attr_js = js["attributes"];
        for (const auto& attr : attrs) {
            nlohmann::json a_js;
            a_js["attribute_path"] = attr;
            a_js["base_attribute_path"] = position;
            a_js["type"] = attr;
            util_attr_js.emplace_back(a_js);
            attr_js[attr] = "all";
        }


        exit_mode = run_js(argv[0], js, name_spec_file, json_integration_config_file);
    }


    assert(exit_mode != -1); // "Some subcommand should have updated the exit mode"
    return exit_mode;
}
