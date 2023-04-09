#include "Models.h" 
#include <filesystem>
#include <nlohmann/json.hpp>
#include <fstream>

namespace extremeopt::tests {
//singleton pattern that computes 
namespace {
std::map<std::string, ModelFiles> generate_model_file_data() {

    std::filesystem::path data_base_path(WMT_DATA_DIR);
    assert(std::filesystem::exists(data_base_path));
    std::filesystem::path seamed_path = data_base_path / "seamed";
    std::filesystem::path file_list_file = seamed_path / "files.js";
    std::ifstream ifs(file_list_file);
    nlohmann::json files;
    ifs >> files;

    std::map<std::string, ModelFiles> ret;
    for(const auto& [modelname,files]: files.items()) {
        ModelFiles r;
        r.obj_filepath = seamed_path / files["filename"];
        if (files.contains("EEfilename")) {
            r.constraint_filepath = seamed_path / files["EEfilename"];
        }
        ret[modelname] = std::move(r);
    }

    return ret;

}
const static std::map<std::string, ModelFiles> all_model_files = generate_model_file_data();
}

    // returns a sequence of model names
    std::vector<std::string> get_model_names() {
        std::vector<std::string> ret;
        ret.reserve(all_model_files.size());
        for(const auto& [filename,data]: all_model_files) {
            ret.emplace_back(filename);
        }
        return ret;
    }


    // returns the obj file and the EE file (if it exists)
    ModelFiles get_model_files(const std::string& name) {
        return all_model_files.at(name);
    }

    //returns all of the model files we have as pairs of example names and 
    std::map<std::string,ModelFiles> get_all_model_files() {
        return all_model_files;
    }


}
