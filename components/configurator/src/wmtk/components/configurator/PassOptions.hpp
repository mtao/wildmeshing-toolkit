#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>


namespace wmtk::components::configurator {

struct PassOptions
{
    std::string mesh_path;
    int64_t iterations = 10;
    std::vector<std::string> operations;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PassOptions)
};


} // namespace wmtk::components::configurator
