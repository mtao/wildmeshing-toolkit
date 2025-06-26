#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace wmtk::applications::utils {
    std::filesystem::path get_integration_test_data_root(const std::filesystem::path& js_path, const std::filesystem::path& app_path) {
        spdlog::info("Adding integration test root at [{}]", std::filesystem::absolute(js_path).string());
        std::ifstream ifs(js_path);
        nlohmann::json js;
        ifs >> js;
        return js[app_path.filename()]["data_folder"];


    }
}
