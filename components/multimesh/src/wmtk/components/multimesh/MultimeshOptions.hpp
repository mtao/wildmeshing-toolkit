#pragma once

#include "utils/AttributeDescription.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/components/utils/json_macros.hpp>

namespace wmtk::components::multimesh {


struct MultimeshBijectionOptions {
    std::string parent_path;
    std::string child_path;
WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshBijectionOptions)                                   
};

struct MultimeshBoundaryOptions {
    std::string mesh_path;
    int8_t boundary_dimension;

    // optional attribute to store the boundary data
    utils::AttributeDescription boundary_attribute;
WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshBoundaryOptions)                                   
};
struct MultimeshAxisAlignedFusionOptions {

    std::string mesh_path;
    std::vector<bool> axes_to_fuse;
    double epsilon;

WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshAxisAlignedFusionOptions)                                   
};
struct MultimeshTagOptions {
    std::string mesh_path;
    utils::AttributeDescription tag_attribute;
WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshTagOptions)                                   
};
struct MultimeshOptions {
    std::string type;

    std::variant<MultimeshBijectionOptions,MultimeshBoundaryOptions, MultimeshAxisAlignedFusionOptions, MultimeshTagOptions> options;
WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshOptions)                                   
};






//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MultimeshUVOptions, type, parent, child, name);
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MultimeshBOptions, type, name, mesh, position, tag_name);
//NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//    MultimeshTOptions,
//    type,
//    name,
//    mesh,
//    position,
//    tag,
//    tag_value,
//    primitive);

} // namespace wmtk::components
