#include "read_inputs.hpp"
#include <nlohmann/json.hpp>
#include "wmtk/components/input/InputOptions.hpp"
#include "wmtk/components/input/input.hpp"
#include "wmtk/components/utils/PathResolver.hpp"
#include <wmtk/components/multimesh/MultimeshOptions.hpp>
#include <wmtk/components/multimesh/multimesh.hpp>
namespace wmtk::applications::utils {

components::multimesh::MeshCollection read_inputs(
    const nlohmann::json& js,
    std::string_view input_tag,
    std::string_view path_resolver_tag,
    const std::span<std::filesystem::path>& additional_paths)
{
    wmtk::components::multimesh::MeshCollection meshes;
    components::utils::PathResolver path_resolver;
    path_resolver.add_path(".");

    if (js.contains(path_resolver_tag)) {
        path_resolver = js[path_resolver_tag];
    }
    for (const auto& p : additional_paths) {
        path_resolver.add_path(p);
    }

    auto add = [&](const auto& my_input_js) {
        auto input_opts = my_input_js.template get<wmtk::components::input::InputOptions>();
        auto& named_mesh =
            meshes.add_mesh(wmtk::components::input::input(input_opts, path_resolver));

        if (my_input_js.contains("multimesh")) {
            const nlohmann::ordered_json mm_js = my_input_js["multimesh"];
            if (mm_js.is_array()) {
                for (const auto& single_mm : mm_js) {
                    wmtk::components::multimesh::multimesh(meshes, single_mm);
                }
            } else {
                wmtk::components::multimesh::multimesh(meshes, mm_js);
            }
        }
    };

    const auto& input_js = js["input"];
    if (input_js.is_array()) {
        for (const auto& js : input_js) {
            add(js);
        }
    } else {
        add(input_js);
    }
    return meshes;
}
} // namespace wmtk::applications::utils
