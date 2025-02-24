#pragma once

#include <memory>
#include <nlohmann/json.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "axis_aligned_fusion.hpp"
#include "from_boundary.hpp"
#include "from_facet_bijection.hpp"
#include "from_tag.hpp"

namespace wmtk {
class Mesh;

}
namespace wmtk::components::multimesh {
class MeshCollection;


struct MultimeshOptions : public MultimeshRunnableOptions
{
    std::string type;

    std::shared_ptr<MultimeshRunnableOptions> options;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshOptions)

    void run(MeshCollection& mc) const final;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json&) final;
};


// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MultimeshUVOptions, type, parent, child, name);
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(MultimeshBOptions, type, name, mesh, position, tag_name);
// NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(
//     MultimeshTOptions,
//     type,
//     name,
//     mesh,
//     position,
//     tag,
//     tag_value,
//     primitive);

} // namespace wmtk::components::multimesh
