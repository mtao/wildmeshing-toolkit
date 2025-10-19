
#include "cast_value_variant.hpp"
namespace wmtk::attribute::utils {
namespace {
template <typename T>
auto cast_value_variant(const MeshAttributeHandle::ValueVariant& v) -> T
{
    return std::visit(
        [](const auto& vv) noexcept -> T {
            using VT = std::decay_t<decltype(vv)>;
            if constexpr (std::is_same_v<VT, Rational> && !std::is_same_v<T, Rational>) {
                return static_cast<T>(static_cast<double>(vv));
            } else {
                return static_cast<T>(vv);
            }
        },
        v);
}
} // namespace
MeshAttributeHandle::ValueVariant cast_value_variant(
    const MeshAttributeHandle::ValueVariant& v,
    const AttributeType& type)
{
    using AT = AttributeType;
    switch (type) {
#define ENTRY(TYPE) \
    case TYPE: return cast_value_variant<type_from_attribute_type_enum_t<TYPE>>(v);
        ENTRY(AT::Char);
        ENTRY(AT::Double);
        ENTRY(AT::Int64);
        ENTRY(AT::Rational);
#undef ENTRY
    default: assert(false);
    }
}

} // namespace wmtk::attribute::utils
