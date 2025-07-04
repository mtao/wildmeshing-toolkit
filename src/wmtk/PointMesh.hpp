#pragma once

#include "MeshCRTP.hpp"
#include "Tuple.hpp"

#include <Eigen/Core>

namespace wmtk {
// Simple mesh without topology. Mainly useful for testing attributes without having to construct
// topologies
class PointMesh : public MeshCRTP<PointMesh>
{
private:
    Tuple vertex_tuple_from_id(int64_t id) const;

public:
    friend class MeshCRTP<PointMesh>;
    template <typename U, typename MeshType, typename AT, int Dim>
    friend class attribute::Accessor;
    PointMesh();
    ~PointMesh() override;
    PointMesh(int64_t size);
    PointMesh(const PointMesh& o) = delete;
    PointMesh(PointMesh&& o) = default;
    PointMesh& operator=(const PointMesh& o) = delete;
    PointMesh& operator=(PointMesh&& o) = default;

    [[noreturn]] Tuple switch_tuple(const Tuple& tuple, PrimitiveType type) const override;
    bool is_ccw(const Tuple& tuple) const override;
    using Mesh::is_boundary;
    bool is_boundary(PrimitiveType pt, const Tuple& tuple) const override;
    bool is_boundary_vertex(const Tuple& tuple) const;

    void initialize(int64_t count);


    bool is_valid(const Tuple& tuple) const final override;

    bool is_connectivity_valid() const override { return true; }

    std::vector<std::vector<TypedAttributeHandle<int64_t>>> connectivity_attributes()
        const override;

    std::vector<Tuple> orient_vertices(const Tuple& tuple) const override;

    using MeshCRTP<PointMesh>::id; // getting the (simplex) prototype
    int64_t id(const Tuple& tuple, PrimitiveType type) const;
    int64_t id(int64_t global_id, int8_t orientation, PrimitiveType pt) const
    {
        assert(pt == PrimitiveType::Vertex);
        return global_id;
    }

protected:
    /**
     * @brief internal function that returns the tuple of requested type, and has the global index
     * cid
     *
     * @param gid
     * @return Tuple
     */
    Tuple tuple_from_id(const PrimitiveType type, const int64_t gid) const override;
};

inline int64_t PointMesh::id(const Tuple& tuple, PrimitiveType type) const
{
    switch (type) {
    case PrimitiveType::Vertex: return tuple.global_cid();
    case PrimitiveType::Edge:
    case PrimitiveType::Triangle:
    case PrimitiveType::Tetrahedron:
    default: assert(false); // "Tuple switch: Invalid primitive type"
    }

    return -1;
}
} // namespace wmtk
