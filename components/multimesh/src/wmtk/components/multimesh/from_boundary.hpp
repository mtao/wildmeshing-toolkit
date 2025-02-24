#pragma once

#include <memory>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/components/utils/json_macros.hpp>
#include "MultimeshRunnableOptions.hpp"
#include "utils/AttributeDescription.hpp"

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;
}
} // namespace wmtk
namespace wmtk::components::multimesh {
class MeshCollection;

struct MultimeshBoundaryOptions : public MultimeshRunnableOptions
{
    std::string mesh_path;
    int8_t boundary_dimension;

    // optional attribute to store the boundary data
    // if the attribute is empty then it'll be named boundary_tag and then deleted
    utils::AttributeDescription boundary_attribute;
    char boundary_attribute_value = 1;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshBoundaryOptions)
    void run(MeshCollection&) const final;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json&) final;
};

// returns new mesh created from the boundary
std::shared_ptr<Mesh> from_boundary(
    Mesh& m,
    const PrimitiveType boundary_dimension,
    const std::string& boundary_attribute_name,
    char value = 1,
    const std::vector<wmtk::attribute::MeshAttributeHandle>& passed_attributes = {});

} // namespace wmtk::components::multimesh
