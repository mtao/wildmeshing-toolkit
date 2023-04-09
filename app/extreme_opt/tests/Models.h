#pragma once
#include <filesystem>
#include <optional>
#include <vector>
#include <map>


namespace extremeopt::tests {


struct ModelFiles
{
    std::filesystem::path obj_filepath;
    std::optional<std::filesystem::path> constraint_filepath;
};

// returns a sequence of model names
std::vector<std::string> get_model_names();


// returns the obj file and the EE file (if it exists)
ModelFiles get_model_files(const std::string& name);

// returns all of the model files we have as pairs of example names and
std::map<std::string, ModelFiles> get_all_model_files();

} // namespace extremeopt::tests
