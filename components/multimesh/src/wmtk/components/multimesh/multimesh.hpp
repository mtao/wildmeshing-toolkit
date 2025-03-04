#pragma once

#include <memory>

namespace wmtk {
class Mesh;
namespace attribute {
class MeshAttributeHandle;
}
} // namespace wmtk
namespace wmtk::components::multimesh {
class MeshCollection;
struct MultimeshOptions;

enum class MultiMeshType { UV, Boundary, Tag };

std::pair<std::shared_ptr<Mesh>, std::shared_ptr<Mesh>> multimesh(
    const MultiMeshType& type,
    Mesh& parent,
    std::shared_ptr<Mesh> child,
    const attribute::MeshAttributeHandle& parent_position_handle,
    const std::string& tag_name,
    const int64_t tag_value,
    const int64_t primitive);


void multimesh(MeshCollection& mc, const MultimeshOptions& options);
} // namespace wmtk::components::multimesh
