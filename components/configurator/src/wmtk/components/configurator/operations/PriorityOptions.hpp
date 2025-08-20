#pragma once
#include <nlohmann/json_fwd.hpp>
#include <string>
#include <wmtk/components/utils/json_macros.hpp>
namespace wmtk::components::multimesh {
class MeshCollection;
}
namespace wmtk {
namespace operations {
class Operation;
}
} // namespace wmtk

namespace wmtk::components::configurator::operations {

struct PriorityOptions
{
    std::string type = {};
    std::string attribute_path = {}; // TODO move this into a child
    bool minimize = false; // if true prioritizes teh largest priority operations first
    static PriorityOptions create(const nlohmann::json& js);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PriorityOptions)

    void assign_to(
        const wmtk::components::multimesh::MeshCollection& mc,
        wmtk::operations::Operation&) const;

    bool operator==(const PriorityOptions& o) const = default;
    operator bool() const { return !type.empty(); }
};
} // namespace wmtk::components::configurator::operations
