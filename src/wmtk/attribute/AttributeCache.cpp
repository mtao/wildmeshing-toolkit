#include "AttributeCache.hpp"
#include <wmtk/utils/Rational.hpp>

namespace wmtk::attribute {


template <typename T>
AttributeCache<T>::AttributeCache() = default;
template <typename T>
AttributeCache<T>::~AttributeCache() = default;
template <typename T>
auto AttributeCache<T>::load_it(long index) const -> std::pair<typename DataStorage::iterator, bool>
{
    if (const auto& it = m_indices.find(index); it != m_indices.end()) {
        return {it, false};
    } else {
        return m_indices.try_emplace(index, false);
    }
}


template <typename T>
void AttributeCache<T>::clear()
{
    m_data.clear();
    m_indices.clear();
}

template <typename T>
void AttributeCache<T>::flush_to(Attribute<T>& attribute)
{
    for (auto& [index, data] : m_data) {
        if (std::get<1>(data)) {
            attribute.vector_attribute(index) = data_as_const(std::get<0>(data));
        }
        data.dirty = false;
    }
}
template <typename T>
void AttributeCache<T>::flush_to(AttributeCache<T>& other)
{
    auto& o_data = other.m_data;

    for (auto& [index, data] : m_data) {
        if (data.dirty) {
            o_data[index] = data;
            other.data(index) = data_as_const(std::get<0>(data));
        }
        data.dirty = false;
    }
}

template <typename T>
auto AttributeCacheData<T>::vector_data_local(size_t index, long stride) -> typename Vector::MapType
{
    return typename Vector::MapType(m_data.data() + stride * index, stride);
}
template <typename T>
auto AttributeCacheData<T>::const_vector_data_local(size_t index, long stride) const -> typename Vector::ConstMapType
{
    return typename Vector::ConstMapType(m_data.data() + stride * index, stride);
}

template <typename T>
auto AttributeCacheData<T>::vector_data_local(size_t index, long stride) -> typename Vector::MapType
{
    return typename Vector::MapType(m_data.data() + stride * index, stride);
}
template <typename T>
auto AttributeCacheData<T>::const_vector_data_local(size_t index, long stride) const -> typename Vector::ConstMapType
{
    return typename Vector::ConstMapType(m_data.data() + stride * index, stride);
}
template class AttributeCache<long>;
template class AttributeCache<double>;
template class AttributeCache<char>;
template class AttributeCache<Rational>;
} // namespace wmtk
