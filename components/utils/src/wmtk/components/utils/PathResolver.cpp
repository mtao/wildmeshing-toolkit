#include "PathResolver.hpp"
#include "json_utils.hpp"
#include "wmtk/utils/Logger.hpp"


namespace fs = std::filesystem;
namespace wmtk::components::utils {

std::pair<std::filesystem::path, bool> PathResolver::try_resolving_path(
    const std::filesystem::path& potential_base,
    const std::filesystem::path& path)
{
    if (path.is_absolute()) {
        bool exists = fs::exists(path);
        logger().debug("PathResolver resolved an absolute path [{}] - does it exists? {}", path.string(), exists);
        return {path, exists};
    }

    const fs::path root_abs = fs::absolute(potential_base);

    if (!fs::is_directory(root_abs)) {
        logger().debug("PathResolver failed to resolved a potential base path [{}] because it is not a directory", potential_base.string());
        return {path, false};
    }


    const fs::path resolved_path = fs::weakly_canonical(potential_base / path);

    bool exists =  fs::exists(resolved_path);
    if(exists) {
        logger().debug("PathResolver resolved a potential base path [{}] with [{}] to form [{}]", potential_base.string(), path.string(), resolved_path.string());
    } else {
        logger().debug("PathResolver failed to resolve a potential base path [{}] with [{}] to form [{}]", potential_base.string(), path.string(), resolved_path.string());
    }
    return {resolved_path, exists};
}

struct PathResolver::Impl
{
    Impl() = default;
    Impl(Impl&&) = default;
    Impl(const Impl&) = default;
    Impl& operator=(Impl&&) = default;
    Impl& operator=(const Impl&) = default;

    void add_path(const std::filesystem::path& path) { m_paths.emplace_back(path); }

    std::pair<std::filesystem::path, bool> resolve(const std::filesystem::path& path) const
    {
        for (const auto& p : m_paths) {
            logger().debug("PathResolver trying to use base path [{}]", p.string());
            auto res = try_resolving_path(p, path);
            const auto& [new_path, succeeded] = res;
            if (succeeded) {
                return res;
            }
        }
        return {path, false};
    }


    std::vector<std::filesystem::path> m_paths;
};


PathResolver::PathResolver()
    : PathResolver(".")
{}

PathResolver::PathResolver(const std::filesystem::path& path)
    : m_impl(std::make_unique<Impl>())
{
    m_impl->add_path(path);
}
PathResolver::PathResolver(PathResolver&& o) = default;
PathResolver::PathResolver(const PathResolver& o)
    : m_impl(std::make_unique<Impl>(*o.m_impl))
{}
PathResolver& PathResolver::operator=(PathResolver&&) = default;
PathResolver& PathResolver::operator=(const PathResolver& o)
{
    *m_impl = *o.m_impl;
    return *this;
}
PathResolver::~PathResolver() = default;


void PathResolver::clear_paths()
{
    m_impl->m_paths.clear();
}
void PathResolver::add_path(const std::filesystem::path& path)
{
    m_impl->add_path(path);
}

std::pair<std::filesystem::path, bool> PathResolver::resolve(
    const std::filesystem::path& path) const
{
    return m_impl->resolve(path);
}

std::vector<std::filesystem::path> PathResolver::get_paths() const
{
    return m_impl->m_paths;
}

WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PathResolver)
{
    nlohmann_json_j = nlohmann_json_t.m_impl->m_paths;
}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PathResolver)
{
    nlohmann_json_t.m_impl->m_paths.clear();
    if (nlohmann_json_j.is_string()) {
        nlohmann_json_t.add_path(nlohmann_json_j.get<std::filesystem::path>());
    } else if (nlohmann_json_j.is_array()) {
        for (const auto& p : nlohmann_json_j) {
            nlohmann_json_t.add_path(nlohmann_json_j.get<std::filesystem::path>());
        }
    }
}

} // namespace wmtk::components::utils
