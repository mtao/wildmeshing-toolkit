#pragma once

#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexAdjacency.hpp>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/attribute/Accessor.hpp>

namespace wmtk {
class PointMesh;
class EdgeMesh;
class TriMesh;
class TetMesh;

namespace operations::utils {
    class UpdateEdgeOperationMultiMeshMapFunctor;
}
} // namespace wmtk
namespace wmtk::dart {
template <int Dim, typename MeshType>
class DartIndexAccessor
{
public:
    static_assert(sizeof(int64_t) * (Dim + 1) >= sizeof(dart::SimplexAdjacency<Dim>));
    using BaseAccessor = wmtk::attribute::Accessor<int64_t, MeshType, Dim + 1>;

    DartIndexAccessor(BaseAccessor acc)
        : m_base_accessor{std::move(acc)}
    {}

    DartIndexAccessor(const MeshType& mesh, const wmtk::attribute::MeshAttributeHandle& mah)
        : m_base_accessor{mesh.template create_const_accessor<int64_t, Dim + 1>(mah)}
    {}
    DartIndexAccessor(const MeshType& mesh, const wmtk::attribute::TypedAttributeHandle<int64_t>& h)
        : m_base_accessor{mesh.template create_const_accessor<int64_t, Dim + 1>(h)}
    {}
    template <typename MeshType2>
    DartIndexAccessor(const DartIndexAccessor<Dim, MeshType2>& o)
        : m_base_accessor{o.m_base_accessor}
    {}

    dart::SimplexAdjacency<Dim>& operator[](int64_t index)
    {
        return *reinterpret_cast<dart::SimplexAdjacency<Dim>*>(
            m_base_accessor.index_access().vector_attribute(index).data());
    }
    const dart::SimplexAdjacency<Dim>& operator[](int64_t index) const
    {
        return *reinterpret_cast<const dart::SimplexAdjacency<Dim>*>(
            m_base_accessor.index_access().const_vector_attribute(index).data());
    }

    const MeshType& mesh() const { return m_base_accessor.mesh(); }


protected:
    BaseAccessor m_base_accessor;
};

template <int Dim, typename MeshType>
class DartAccessor : public DartIndexAccessor<Dim, MeshType>
{
public:
    using IndexBaseType = DartIndexAccessor<Dim, MeshType>;

protected:
    using IndexBaseType::m_base_accessor;

public:
    using IndexBaseType::IndexBaseType;

    using IndexBaseType::mesh;
    friend class wmtk::operations::utils::UpdateEdgeOperationMultiMeshMapFunctor;


    static wmtk::attribute::TypedAttributeHandle<int64_t>
    register_attribute(MeshType& m, const std::string_view& name, PrimitiveType pt)
    {
        // add one for the orientation pack
        auto handle =
            m.template register_attribute_typed<int64_t>(std::string(name), pt, Dim + 1, false, -1);
        return handle;
    }

    template <typename IT, typename OT>
    dart::SimplexAdjacency<Dim>& operator[](const dart::_Dart<IT, OT>& t)
    {
         return IndexBaseType::operator[](m_base_accessor.index(t));
    }
    template <typename IT, typename OT>
    const dart::SimplexAdjacency<Dim>& operator[](const dart::_Dart<IT, OT>& t) const
    {
        return IndexBaseType::operator[](m_base_accessor.index(t));
    }


};



template <int Dim, typename MeshType = Mesh>
auto register_dart_attribute(MeshType& mesh, const std::string_view& name, PrimitiveType pt)
{
    return DartAccessor<Dim, Mesh>::register_attribute(mesh, name, pt);
}

} // namespace wmtk::attribute
