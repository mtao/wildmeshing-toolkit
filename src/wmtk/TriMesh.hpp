#pragma once

#include <wmtk/autogen/tri_mesh/simplex_index_from_permutation_index.hpp>
#include "MeshCRTP.hpp"
#include "Tuple.hpp"

#include <Eigen/Core>

namespace wmtk {
namespace utils {
class TriMeshInspector;
}
namespace operations::utils {
class MultiMeshEdgeSplitFunctor;
class MultiMeshEdgeCollapseFunctor;
class UpdateEdgeOperationMultiMeshMapFunctor;
} // namespace operations::utils


class TriMesh : public MeshCRTP<TriMesh>
{
public:
    friend class utils::TriMeshInspector;
    friend class MeshCRTP<TriMesh>;
    friend class operations::utils::MultiMeshEdgeCollapseFunctor;
    friend class operations::utils::MultiMeshEdgeSplitFunctor;
    friend class operations::utils::UpdateEdgeOperationMultiMeshMapFunctor;
    template <typename U, typename MeshType, typename AT, int Dim>
    friend class attribute::Accessor;
    using MeshCRTP<TriMesh>::create_accessor;
    using MeshCRTP<TriMesh>::create_const_accessor;
    TriMesh();
    TriMesh(const TriMesh& o) = delete;
    TriMesh& operator=(const TriMesh& o) = delete;
    TriMesh(TriMesh&& o);
    TriMesh& operator=(TriMesh&& o);
    ~TriMesh() override;
    void make_cached_accessors();


    Tuple switch_tuple(const Tuple& tuple, PrimitiveType type) const final override; // CRTP
                                                                                     // override


    bool is_ccw(const Tuple& tuple) const final override; // CRTP override
    using Mesh::is_boundary;
    bool is_boundary(PrimitiveType pt, const Tuple& tuple) const final override; // CRTP override
    bool is_boundary_vertex(const Tuple& tuple) const;
    bool is_boundary_edge(const Tuple& tuple) const;

    void initialize(
        Eigen::Ref<const RowVectors3l> FV,
        Eigen::Ref<const RowVectors3l> FE,
        Eigen::Ref<const RowVectors3l> FF,
        Eigen::Ref<const VectorXl> VF,
        Eigen::Ref<const VectorXl> EF);
    void initialize(Eigen::Ref<const RowVectors3l> F, bool make_free = false);
    void initialize_free(int64_t count);

    using Mesh::is_valid;
    bool is_valid(const Tuple& tuple) const final override;

    bool is_connectivity_valid() const final override;

    std::vector<std::vector<TypedAttributeHandle<int64_t>>> connectivity_attributes()
        const override;

    Tuple switch_vertex(const Tuple& tuple) const;
    Tuple switch_edge(const Tuple& tuple) const;
    Tuple switch_face(const Tuple& tuple) const;

    std::vector<Tuple> orient_vertices(const Tuple& t) const override;

    int64_t id(const Tuple& tuple, PrimitiveType type) const;
    int64_t id(int64_t global_id, int8_t permutation_index, PrimitiveType pt) const;
    using MeshCRTP<TriMesh>::id; // getting the (simplex) prototype

    int64_t id_vertex(const Tuple& tuple) const { return id(tuple, PrimitiveType::Vertex); }
    int64_t id_edge(const Tuple& tuple) const { return id(tuple, PrimitiveType::Edge); }
    int64_t id_face(const Tuple& tuple) const { return id(tuple, PrimitiveType::Triangle); }

protected:
    /**
     * @brief internal function that returns the tuple of requested type, and has the global index
     * cid
     *
     * @param gid
     * @return Tuple
     */
    Tuple tuple_from_id(const PrimitiveType type, const int64_t gid)
        const final override; // CRTP override
    Tuple tuple_from_global_ids(int64_t fid, int64_t eid, int64_t vid) const;

protected:
    attribute::TypedAttributeHandle<int64_t> m_vf_handle;
    attribute::TypedAttributeHandle<int64_t> m_ef_handle;

    attribute::TypedAttributeHandle<int64_t> m_fv_handle;
    attribute::TypedAttributeHandle<int64_t> m_fe_handle;
    attribute::TypedAttributeHandle<int64_t> m_ff_handle;

    std::unique_ptr<attribute::Accessor<int64_t, TriMesh>> m_vf_accessor = nullptr;
    std::unique_ptr<attribute::Accessor<int64_t, TriMesh>> m_ef_accessor = nullptr;
    std::unique_ptr<attribute::Accessor<int64_t, TriMesh>> m_fv_accessor = nullptr;
    std::unique_ptr<attribute::Accessor<int64_t, TriMesh>> m_fe_accessor = nullptr;
    std::unique_ptr<attribute::Accessor<int64_t, TriMesh>> m_ff_accessor = nullptr;


    Tuple vertex_tuple_from_id(int64_t id) const;
    Tuple edge_tuple_from_id(int64_t id) const;
    Tuple face_tuple_from_id(int64_t id) const;


    class TriMeshOperationExecutor;
    static Tuple with_different_cid(const Tuple& t, int64_t cid);
};

inline Tuple TriMesh::switch_vertex(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Vertex);
}
inline Tuple TriMesh::switch_edge(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Edge);
}
inline Tuple TriMesh::switch_face(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Triangle);
}
inline int64_t TriMesh::id(const Tuple& tuple, PrimitiveType type) const
{
    switch (type) {
    case PrimitiveType::Vertex: {
        assert(Mesh::is_valid(tuple));
        int64_t v = m_fv_accessor->const_topological_scalar_attribute(tuple, PrimitiveType::Vertex);
        return v;
    }
    case PrimitiveType::Edge: {
        assert(Mesh::is_valid(tuple));
        int64_t v = m_fe_accessor->const_topological_scalar_attribute(tuple, PrimitiveType::Edge);
        return v;
    }
    case PrimitiveType::Triangle: {
        return tuple.global_cid();
    }
    case PrimitiveType::Tetrahedron: [[fallthrough]];
    default: assert(false); // "Tuple id: Invalid primitive type"
    }

    return -1;
}
inline int64_t TriMesh::id(int64_t global_id, int8_t permutation_index, PrimitiveType pt) const
{
    int8_t index = autogen::tri_mesh::simplex_index_from_permutation_index(permutation_index, pt);
    switch (pt) {
    case PrimitiveType::Vertex: {
        return m_fv_accessor->attribute().const_vector_single_value(global_id,index);
    }
    case PrimitiveType::Edge: {
        return m_fe_accessor->attribute().const_vector_single_value(global_id,index);
    }
    case PrimitiveType::Triangle: {
        return global_id;
    }
    case PrimitiveType::Tetrahedron: [[fallthrough]];
    default: assert(false); // "Tuple id: Invalid primitive type"
    }

    return -1;
}
} // namespace wmtk
