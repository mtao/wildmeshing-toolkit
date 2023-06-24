#include "Accessor.hpp"

#include "Mesh.hpp"
#include "MeshAttributes.hpp"

namespace wmtk {

template <typename T>
Accessor<T>::Accessor(MeshType& mesh, const MeshAttributeHandle<Type>& handle)
    : m_mesh(mesh)
    , m_handle(handle)
{}

template <typename T>
auto Accessor<T>::attributes() -> MeshAttributes<Type>& 
{
    if constexpr(!IsConst) {
        return m_mesh.get_mesh_attributes(m_handle);
    } else {
        assert(false);
        static MeshAttributes<Type> dummy;
        return dummy;
    }
}
template <typename T>
auto Accessor<T>::attributes() const-> const MeshAttributes<Type>& 
{
    return m_mesh.get_mesh_attributes(m_handle);
}

template <typename T>
auto Accessor<T>::vector_attribute(const long index) const -> ConstMapResult
{
    return attributes().vector_attribute(m_handle.m_base_handle, index);
}
template <typename T>
auto Accessor<T>::vector_attribute(const long index) -> MapResult
{
    return attributes().vector_attribute(m_handle.m_base_handle, index);
}

template <typename T>
T Accessor<T>::scalar_attribute(const long index) const
{
    return attributes().scalar_attribute(m_handle.m_base_handle, index);
}

template <typename T>
T& Accessor<T>::scalar_attribute(const long index)
{
    return attributes().scalar_attribute(m_handle.m_base_handle, index);
}
template <typename T>
auto Accessor<T>::vector_attribute(const Tuple& t) const -> ConstMapResult
{
    long index = m_mesh.id(t, m_handle.m_primitive_type);
    return vector_attribute(index);
}
template <typename T>
auto Accessor<T>::vector_attribute(const Tuple& t) -> MapResult
{
    long index = m_mesh.id(t, m_handle.m_primitive_type);
    return vector_attribute(index);
}

template <typename T>
T Accessor<T>::scalar_attribute(const Tuple& t) const
{
    long index = m_mesh.id(t, m_handle.m_primitive_type);
    return scalar_attribute(index);
}

template <typename T>
T& Accessor<T>::scalar_attribute(const Tuple& t)
{
    long index = m_mesh.id(t, m_handle.m_primitive_type);
    return scalar_attribute(index);
}
// template <typename T>
// void MeshAttributes<T>::rollback()
//{
//     attributes()s_copy.clear();
// }
//
// template <typename T>
// void MeshAttributes<T>::begin_protect()
//{
//     attributes()s_copy =
//     attributes()s;
// }
//
// template <typename T>
// void MeshAttributes<T>::end_protect()
//{
//     if (!attributes()s_copy.empty())
//     attributes()s =
//     std::move(attributes()s_copy);
//
//     attributes()s_copy.clear();
// }
//
// template <typename T>
// bool MeshAttributes<T>::is_in_protect() const
//{
//     return !attributes()s_copy.empty();
// }
template class Accessor<char>;
template class Accessor<long>;
template class Accessor<double>;
template class Accessor<const char>;
template class Accessor<const long>;
template class Accessor<const double>;
} // namespace wmtk