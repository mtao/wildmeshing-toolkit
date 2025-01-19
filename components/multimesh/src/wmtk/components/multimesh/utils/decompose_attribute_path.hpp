#include <array>
#include <ranges>
#include "AttributeDescription.hpp"

namespace wmtk::components::multimesh::utils {
// turns an attribute path mesh.path/attrname to mesh.path attrname
// std::array<std::string_view, 2>
inline auto decompose_attribute_path(std::string_view attribute_path)
{
#if defined(WMTK_ENABLED_CPP20)
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
#else

    std::array<std::string, 2> ret;
    std::vector<std::string> tmp;
    if (attribute_path.empty()) {
        tmp.emplace_back("");
        tmp.emplace_back("");

    } else {
        std::string v = std::string(attribute_path);
        std::istringstream iss(v);
        std::string token;
        if (v.size() > 0 && v[0] == '/') {
            tmp.emplace_back("");
        }
        while (std::getline(iss, token, '/')) {
            if (!token.empty()) tmp.emplace_back(token);
        }
        // at most 2 tokens are allowed
        assert(tmp.size() <= 2);
        if (tmp.size() == 1) {
            tmp = {"", tmp[0]};
        }
    }
    return std::array<std::string, 2>{{tmp[0], tmp[1]}};
#endif
}

// std::array<std::string_view, 2>
inline auto decompose_attribute_path(const AttributeDescription& description)
{
    return decompose_attribute_path(description.path);
}
}
