#include "CachingAttribute.hxx"
#include <wmtk/Mesh.hpp>

namespace wmtk::attribute {

#if defined(WMTK_ENABLED_DEV_MODE)
#define CLASS_DEC(TYPE) template class CachingAttribute<TYPE>;
#define VECTOR_DEC(TYPE, D)                                                              \
    template auto CachingAttribute<TYPE>::const_vector_single_value<D>(                  \
        int64_t index,                                                                   \
        int8_t single_index) const -> const TYPE&;                                       \
    template auto CachingAttribute<TYPE>::const_vector_attribute<D>(int64_t index) const \
        -> ConstMapResult<D>;                                                            \
    template auto CachingAttribute<TYPE>::vector_attribute<D>(int64_t index) -> MapResult<D>;


#define DEC(TYPE)        \
    CLASS_DEC(TYPE)      \
    VECTOR_DEC(TYPE, -1) \
    VECTOR_DEC(TYPE, 1)  \
    VECTOR_DEC(TYPE, 2)  \
    VECTOR_DEC(TYPE, 3)  \
    VECTOR_DEC(TYPE, 4)  \
    VECTOR_DEC(TYPE, 5)  \
    VECTOR_DEC(TYPE, 6)


DEC(double)
DEC(int64_t)
DEC(Rational)
DEC(char)
#endif

template <typename T>
void CachingAttribute<T>::serialize(int dim, io::MeshWriter& writer) const
{
    if (has_transactions()) {
        throw std::runtime_error("Cannot serialize a mesh while there are active transactions");
    }
    Attribute<T>::serialize(dim, writer);
}

template void CachingAttribute<int64_t>::serialize(int dim, io::MeshWriter& writer) const;
template void CachingAttribute<double>::serialize(int dim, io::MeshWriter& writer) const;
template void CachingAttribute<char>::serialize(int dim, io::MeshWriter& writer) const;
template void CachingAttribute<wmtk::Rational>::serialize(int dim, io::MeshWriter& writer) const;

} // namespace wmtk::attribute
