#include "read_inputs.hpp"
#include <nlohmann/json.hpp>
#include "wmtk/components/input/InputOptions.hpp"
#include "wmtk/components/input/input.hpp"
#include "wmtk/components/utils/PathResolver.hpp"
namespace wmtk::applications::utils {

components::multimesh::MeshCollection read_inputs(
    const nlohmann::json& js,
    std::string_view input_tag,
    std::string_view path_resolver_tag,
    const std::span<std::filesystem::path>& additional_paths)
{
    wmtk::components::multimesh::MeshCollection meshes;
    components::utils::PathResolver path_resolver;

    if (js.contains(path_resolver_tag)) {
        path_resolver = js[path_resolver_tag];
    }
    for (const auto& p : additional_paths) {
        path_resolver.add_path(p);
    }

    if (js["input"].is_array()) {
        for (const auto& in_opts_js : js["input"]) {
            wmtk::components::input::InputOptions opts = in_opts_js;
            meshes.add_mesh(wmtk::components::input::input(opts, path_resolver));
        }
    } else {
        wmtk::components::input::InputOptions opts = js["input"];
        meshes.add_mesh(wmtk::components::input::input(opts, path_resolver));
    }
    return meshes;
}
} // namespace wmtk::applications::utils
