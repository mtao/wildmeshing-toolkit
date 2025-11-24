#pragma once

#include <filesystem>
#include <span>
#include "wmtk/components/multimesh/MeshCollection.hpp"
namespace wmtk::applications::utils {


/** Creates the meshes declared in the input segment taking into acount additional paths
 * Note that if the path resolver tag is set then this function will strictly follow the settings in
 * that path + any additional paths passed in Extracts input meshes from an application's json
 */
auto read_inputs(
    const nlohmann::json& application_js, //!< JSON for the entire application
    std::string_view input_tag = "input", //!< tag in the application js that contains the input
    std::string_view path_resolver_tag =
        "root", //!< Standard tag for finding an extra path in integration tests
    const std::span<std::filesystem::path>& additional_paths = {}
    //!< any additional paths we might want
    ) -> components::multimesh::MeshCollection;

} // namespace wmtk::applications::utils
