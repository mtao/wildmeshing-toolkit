#pragma once 



namespace wmtk {
    class Mesh;

namespace multimesh::utils::internal {
    // @input parent
    // @input child a mesh that lies below parent in a submesh
    void map_twice_includes_itself(const Mesh& parent, const Mesh& child);
}
}
