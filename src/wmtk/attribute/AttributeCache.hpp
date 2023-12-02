#pragma once
#include <Eigen/Core>
#include <map>
#include <memory>
#include "AccessorBase.hpp"
#include "AttributeCacheData.hpp"


namespace wmtk::attribute {
template <typename T>
class AttributeCache
{
public:
    using Vector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
    using Data = std::tuple<size_t, bool>;
    using DataStorage = std::map<long, Data>;

    using MapResult = typename AccessorBase<T>::MapResult;
    using ConstMapResult = typename AccessorBase<T>::ConstMapResult;


    AttributeCache();
    ~AttributeCache();

    // returns an iterator and if the value was inserted
    // the returned value may have undetermined state if new oen was inserted
    std::pair<typename DataStorage::iterator, bool> load_it(long index) const;


    void clear();

    void flush_to(Attribute<T>& attribute);
    void flush_to(AttributeCache<T>& other);

    typename Vector::MapType vector_data(size_t index, long stride);
    typename Vector::ConstMapType const_vector_data(size_t index, long stride) const;


    T& scalar_data(size_t index);
    const T& const_scalar_data(size_t index) const;


    typename Vector::MapType vector_data_local(size_t index, long stride);
    typename Vector::ConstMapType const_vector_data_local(size_t index, long stride) const;


    T& scalar_data_local(size_t index);
    const T& const_scalar_data_local(size_t index) const;


protected:
    mutable std::vector<T> m_data;
    mutable DataStorage m_indices;
};
} // namespace wmtk
