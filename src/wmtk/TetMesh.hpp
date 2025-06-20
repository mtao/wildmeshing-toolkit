#pragma once

#include <wmtk/autogen/tet_mesh/simplex_index_from_permutation_index.hpp>
#include "MeshCRTP.hpp"

namespace wmtk {
namespace operations::utils {
class MultiMeshEdgeSplitFunctor;
class MultiMeshEdgeCollapseFunctor;
class UpdateEdgeOperationMultiMeshMapFunctor;
} // namespace operations::utils
class TetMesh : public MeshCRTP<TetMesh>
{
public:
    friend class MeshCRTP<TetMesh>;
    friend class operations::utils::MultiMeshEdgeSplitFunctor;
    friend class operations::utils::MultiMeshEdgeCollapseFunctor;
    friend class operations::utils::UpdateEdgeOperationMultiMeshMapFunctor;
    template <typename U, typename MeshType, typename AT, int Dim>
    friend class attribute::Accessor;
    TetMesh();
    ~TetMesh() override;
    TetMesh(const TetMesh& o) = delete;
    TetMesh(TetMesh&& o);
    TetMesh& operator=(const TetMesh& o) = delete;
    TetMesh& operator=(TetMesh&& o);

    Tuple switch_tuple(const Tuple& tuple, PrimitiveType type) const final override;
    bool is_ccw(const Tuple& tuple) const final override;
    using Mesh::is_boundary;
    bool is_boundary(PrimitiveType pt, const Tuple& tuple) const final override;
    bool is_boundary_vertex(const Tuple& tuple) const;
    bool is_boundary_edge(const Tuple& tuple) const;
    bool is_boundary_face(const Tuple& tuple) const;

    bool is_valid(const Tuple& tuple) const final override;

    void initialize(
        Eigen::Ref<const RowVectors4l> TV,
        Eigen::Ref<const RowVectors6l> TE,
        Eigen::Ref<const RowVectors4l> TF,
        Eigen::Ref<const RowVectors4l> TT,
        Eigen::Ref<const VectorXl> VT,
        Eigen::Ref<const VectorXl> ET,
        Eigen::Ref<const VectorXl> FT);
    void initialize(Eigen::Ref<const RowVectors4l> T, bool is_free = false);
    void initialize_free(int64_t count);

    bool is_connectivity_valid() const final override;

    std::vector<std::vector<TypedAttributeHandle<int64_t>>> connectivity_attributes()
        const final override;

    Tuple switch_vertex(const Tuple& tuple) const;
    Tuple switch_edge(const Tuple& tuple) const;
    Tuple switch_face(const Tuple& tuple) const;
    Tuple switch_tetrahedron(const Tuple& tuple) const;

    // get the four vertex tuples in the tv matrix with the same order
    std::vector<Tuple> orient_vertices(const Tuple& t) const override;


    int64_t id(const Tuple& tuple, PrimitiveType type) const;
    int64_t id(int64_t global_id, int8_t permutation_index, PrimitiveType pt) const;
    using MeshCRTP<TetMesh>::id; // getting the (simplex) prototype


    int64_t id_vertex(const Tuple& tuple) const { return id(tuple, PrimitiveType::Vertex); }
    int64_t id_edge(const Tuple& tuple) const { return id(tuple, PrimitiveType::Edge); }
    int64_t id_face(const Tuple& tuple) const { return id(tuple, PrimitiveType::Triangle); }
    int64_t id_tet(const Tuple& tuple) const { return id(tuple, PrimitiveType::Tetrahedron); }

protected:
    void make_cached_accessors();
    /**
     * @brief internal function that returns the tuple of requested type, and has the global index
     * cid
     *
     * @param gid
     * @return Tuple
     */
    Tuple tuple_from_id(const PrimitiveType type, const int64_t gid) const final override;
    Tuple tuple_from_global_ids(int64_t tid, int64_t fid, int64_t eid, int64_t vid) const;

    // private:
protected:
    class TetMeshOperationExecutor;
    TypedAttributeHandle<int64_t> m_vt_handle;
    TypedAttributeHandle<int64_t> m_et_handle;
    TypedAttributeHandle<int64_t> m_ft_handle;

    TypedAttributeHandle<int64_t> m_tv_handle;
    TypedAttributeHandle<int64_t> m_te_handle;
    TypedAttributeHandle<int64_t> m_tf_handle;
    TypedAttributeHandle<int64_t> m_tt_handle;

    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_vt_accessor;
    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_et_accessor;
    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_ft_accessor;

    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_tv_accessor;
    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_te_accessor;
    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_tf_accessor;
    std::unique_ptr<attribute::Accessor<int64_t, TetMesh>> m_tt_accessor;

public:
    Tuple vertex_tuple_from_id(int64_t id) const;
    Tuple edge_tuple_from_id(int64_t id) const;
    Tuple face_tuple_from_id(int64_t id) const;
    Tuple tet_tuple_from_id(int64_t id) const;
};

inline Tuple TetMesh::switch_vertex(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Vertex);
}
inline Tuple TetMesh::switch_edge(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Edge);
}
inline Tuple TetMesh::switch_face(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Triangle);
}
inline Tuple TetMesh::switch_tetrahedron(const Tuple& tuple) const
{
    return switch_tuple(tuple, PrimitiveType::Tetrahedron);
}

inline int64_t TetMesh::id(const Tuple& tuple, PrimitiveType type) const
{
    switch (type) {
    case PrimitiveType::Vertex: {
        auto tv = m_tv_accessor->const_vector_attribute<4>(tuple);
        return tv(tuple.local_vid());
        break;
    }
    case PrimitiveType::Edge: {
        auto te = m_te_accessor->const_vector_attribute<6>(tuple);
        return te(tuple.local_eid());
        break;
    }
    case PrimitiveType::Triangle: {
        auto tf = m_tf_accessor->const_vector_attribute<4>(tuple);
        return tf(tuple.local_fid());
        break;
    }
    case PrimitiveType::Tetrahedron: {
        return tuple.global_cid();
        break;
    }
    default: assert(false); // "Tuple id: Invalid primitive type"
    }

    return -1;
}

inline int64_t TetMesh::id(int64_t global_id, int8_t permutation_index, PrimitiveType pt) const
{
    int8_t index = autogen::tet_mesh::simplex_index_from_permutation_index(permutation_index, pt);
    switch (pt) {
    case PrimitiveType::Vertex: {
        return m_tv_accessor->attribute().const_vector_single_value(global_id,index);
    }
    case PrimitiveType::Edge: {
        return m_te_accessor->attribute().const_vector_single_value(global_id,index);
    }
    case PrimitiveType::Triangle: {
        return m_tf_accessor->attribute().const_vector_single_value(global_id,index);
    }
    case PrimitiveType::Tetrahedron: {
        return global_id;
        break;
    }
    default: assert(false); // "Tuple id: Invalid primitive type"
    }

    return -1;
}

} // namespace wmtk
