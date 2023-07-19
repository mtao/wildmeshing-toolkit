#pragma once
#include <wmtk/TriMesh.hpp>

namespace wmtk::tests {
class DEBUG_TriMesh : public TriMesh
{
public:
    auto edge_tuple_between_v1_v2(const long v1, const long v2, const long fid) const -> Tuple
    {
        ConstAccessor<long> fv = create_accessor<long>(m_fv_handle);
        auto fv_base = create_base_accessor<long>(m_fv_handle);
        Tuple face = face_tuple_from_id(fid);
        auto fv0 = fv.vector_attribute(face);
        REQUIRE(fv0 == fv_base.vector_attribute(fid));
        long local_vid1 = -1, local_vid2 = -1;
        for (long i = 0; i < fv0.size(); ++i) {
            if (fv0[i] == v1) {
                local_vid1 = i;
            }
            if (fv0[i] == v2) {
                local_vid2 = i;
            }
        }
        return Tuple(local_vid1, (3 - local_vid1 - local_vid2) % 3, -1, fid, 0);
    }

    template <typename T>
    AccessorBase<T> create_base_accessor(const MeshAttributeHandle<T>& handle)
    {
        return AccessorBase<T>(*this, handle);
    }

    template <typename T>
    AccessorBase<T> create_const_base_accessor(const MeshAttributeHandle<T>& handle) const
    {
        return AccessorBase<T>(const_cast<DEBUG_TriMesh&>(*this), handle);
    }
    template <typename T>
    AccessorBase<T> create_base_accessor(const MeshAttributeHandle<T>& handle) const
    {
        return create_const_base_accessor(handle);
    }

    const MeshAttributeHandle<long>& f_handle(const PrimitiveType type) const
    {
        switch (type) {
        case PrimitiveType::Vertex: return m_fv_handle;
        case PrimitiveType::Edge: return m_fe_handle;
        case PrimitiveType::Face: return m_ff_handle;
        default: throw std::runtime_error("Invalid PrimitiveType");
        }
    }

    const MeshAttributeHandle<long>& vf_handle() const { return m_vf_handle; }

    const MeshAttributeHandle<long>& ef_handle() const { return m_ef_handle; }


    void reserve_attributes(PrimitiveType type, long size) { Mesh::reserve_attributes(type, size); }
};
} // namespace wmtk::tests