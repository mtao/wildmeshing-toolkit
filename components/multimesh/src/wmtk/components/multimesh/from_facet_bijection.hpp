#pragma once
#include <wmtk/components/utils/json_macros.hpp>
#include "MultimeshRunnableOptions.hpp"

namespace wmtk {
class Mesh;
} // namespace wmtk
namespace wmtk::components::multimesh {

class MeshCollection;

void from_facet_bijection(Mesh& parent, Mesh& child);

struct MultimeshBijectionOptions : public MultimeshRunnableOptions
{
    std::string parent_path;
    std::string child_path;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshBijectionOptions)

    void run(MeshCollection& mc) const final;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json&) final;
};
} // namespace wmtk::components::multimesh
