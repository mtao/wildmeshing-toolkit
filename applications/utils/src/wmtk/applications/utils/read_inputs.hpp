#pragma once

#include <filesystem>
#include <span>
#include "wmtk/components/multimesh/MeshCollection.hpp"
namespace wmtk::applications::utils {


components::multimesh::MeshCollection read_inputs(
    const nlohmann::json& js,
    std::string_view input_tag = "input",
    std::string_view path_resolver_tag = "root",
    const std::span<std::filesystem::path>& additional_paths = {});
}
