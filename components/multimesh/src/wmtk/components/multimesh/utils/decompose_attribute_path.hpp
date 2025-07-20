#include <array>
#include <ranges>
#include "AttributeDescription.hpp"

namespace wmtk::components::multimesh::utils {
// turns an attribute path mesh.path/attrname to mesh.path attrname
// std::array<std::string_view, 2>
inline auto decompose_attribute_path(std::string_view attribute_path)
{
    using namespace std;
    auto tmp = std::ranges::views::split(attribute_path, "/"sv) |
               std::views::transform([](const auto& r) noexcept -> std::string_view {
                   return std::string_view(r.begin(), r.end());
               });

    std::array<std::string_view, 2> ret;
    std::ranges::copy(tmp, ret.begin());
    if (ret[1].empty()) {
        std::swap(ret[0], ret[1]);
    }
    return ret;
}

// std::array<std::string_view, 2>
inline auto decompose_attribute_path(const AttributeDescription& description)
{
    return decompose_attribute_path(description.path);
}
inline auto get_mesh_path_from_attribute_path(auto&& attribute)
{
    return std::get<0>(decompose_attribute_path(attribute));
}
inline auto get_attribute_name_from_attribute_path(auto&& attribute)
{
    return std::get<1>(decompose_attribute_path(attribute));
}
}
