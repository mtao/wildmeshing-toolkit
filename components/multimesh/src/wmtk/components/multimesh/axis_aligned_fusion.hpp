#pragma once

#include <memory>
#include <vector>
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

struct MultimeshAxisAlignedFusionOptions : public MultimeshRunnableOptions
{
    ~MultimeshAxisAlignedFusionOptions() override;
    utils::AttributeDescription attribute_path;
    std::string fused_mesh_name; // name for the mesh created
    std::vector<bool> axes_to_fuse;
    double epsilon = 0;

    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(MultimeshAxisAlignedFusionOptions)
    void run(MeshCollection&) const final;
    void to_json(nlohmann::json& j) const final;
    void from_json(const nlohmann::json&) final;
};
// TODO: this really shouldn't require returning a mesh
// void axis_aligned_fusion(Mesh& m, const std::vector<bool>& axes_to_fuse, double eps = 1e-10);
std::shared_ptr<Mesh>
axis_aligned_fusion(const Mesh& m, const std::vector<bool>& axes_to_fuse, double eps = 1e-10);
std::shared_ptr<Mesh> axis_aligned_fusion(
    const attribute::MeshAttributeHandle& m,
    const std::vector<bool>& axes_to_fuse,
    double eps = 1e-10);


std::shared_ptr<Mesh> axis_aligned_fusion(
    MeshCollection& mc,
    const MultimeshAxisAlignedFusionOptions& fus);

} // namespace wmtk::components::multimesh
