#include "PriorityOptions.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
namespace wmtk::components::configurator::operations {
WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(PriorityOptions){
    WMTK_NLOHMANN_ASSIGN_TYPE_TO_JSON(type, attribute_path, minimize)
    //
}

WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(PriorityOptions)
{
    WMTK_NLOHMANN_JSON_DECLARE_DEFAULT_OBJECT(PriorityOptions);
    WMTK_NLOHMANN_ASSIGN_TYPE_FROM_JSON_WITH_DEFAULT(type, attribute_path, minimize);
}

void PriorityOptions::assign_to(
    const wmtk::components::multimesh::MeshCollection& mc,
    wmtk::operations::Operation& op) const
{
    if (type == "random") {
        op.use_random_priority() = false;
        op.set_priority(nullptr);
    } else if (type == "random") {
        op.use_random_priority() = true;
        op.set_priority(nullptr);
    } else if (type == "attribute") {
        op.use_random_priority() = false;
        auto ap = attribute_path;
        if (!ap.empty()) {
            auto priority_attribute = wmtk::components::multimesh::utils::get_attribute(
                mc,
                wmtk::components::multimesh::utils::AttributeDescription{ap});
            if (minimize) {
                auto priority_func = [priority_attribute](const simplex::Simplex& s) -> double {
                    auto acc =
                        priority_attribute.mesh().create_const_accessor<double>(priority_attribute);
                    return acc.const_scalar_attribute(s);
                };
                op.set_priority(priority_func);
            } else {
                auto priority_func = [priority_attribute](const simplex::Simplex& s) -> double {
                    auto acc =
                        priority_attribute.mesh().create_const_accessor<double>(priority_attribute);
                    return -acc.const_scalar_attribute(s);
                };
                op.set_priority(priority_func);
            }
        }
    } else {
        assert(false); // tried to assign an invalid priority
    }
}
} // namespace wmtk::components::configurator::operations
