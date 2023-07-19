#pragma once
#include <wmtk/PointMesh.hpp>

namespace wmtk::tests {
class DEBUG_PointMesh : public wmtk::PointMesh
{
public:
    using PointMesh::PointMesh;
    long id(const wmtk::Tuple& tup) const
    {
        return PointMesh::id(tup, wmtk::PrimitiveType::Vertex);
    }
    template <typename T>
    AccessorBase<T> create_base_accessor(const MeshAttributeHandle<T>& handle)
    {
        return AccessorBase<T>(*this, handle);
    }

    template <typename T>
    AccessorBase<T> create_const_base_accessor(const MeshAttributeHandle<T>& handle) const
    {
        return AccessorBase<T>(const_cast<DEBUG_PointMesh&>(*this), handle);
    }
};
} // namespace wmtk::tests