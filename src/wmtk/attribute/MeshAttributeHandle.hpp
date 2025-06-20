#pragma once
//
#include <wmtk/utils/Rational.hpp>
//
#include "AttributeType.hpp"
#include "CachingAttribute.hpp"
#include "TypedAttributeHandle.hpp"

#include <tuple>
#include <variant>

namespace wmtk {
class Mesh;
} // namespace wmtk

namespace wmtk::attribute {
template <typename T, typename MeshType, typename AttributeType_, int Dim>
class Accessor;

/* @brief Handle that can construct an accessor on its own
 * NOTE: This naming is inconsistent with the existing
 * AttributeHandle/MeshAttributeHandle nomenclature, but in the future most
 * applications should store MeshAttributeHandles instead of
 * MeshAttributeHandle, and after most of those changes are made we will
 * deprecate that name.
 */
class MeshAttributeHandle
{
public:
    // The variant type for storing all of the tuple types.
    // each type within the Handle variant must have a ::Type value that indicates a type-based
    // descriptor of the data held by the type

    using HandleVariant = std::variant<
        TypedAttributeHandle<char>,
        TypedAttributeHandle<int64_t>,
        TypedAttributeHandle<double>,
        TypedAttributeHandle<wmtk::Rational>>;

    // Convenience class for identifying attribute types
    using ValueVariant = std::variant<char, int64_t, double, wmtk::Rational>;

    using HeldType = AttributeType;

    template <HeldType Type>
    using held_handle_type = std::variant_alternative_t<size_t(Type), HandleVariant>;

    template <HeldType Type>
    using held_primitive_type = typename held_handle_type<Type>::Type;


    // for primitive type attributes returns the held type for primitive types
    // takes as input basic type (char/int64/double/rational)
    // Note this doesn't work with the hybrid type, so this only makes esnse to use in basic
    // attribute storage
    template <typename T>
    constexpr static HeldType held_type_from_primitive();

    template <typename T>
    constexpr static HeldType held_type_from_handle();


    template <typename T>
    constexpr static bool handle_type_is_basic();


    friend class wmtk::Mesh;
    friend class wmtk::hash<MeshAttributeHandle>;
    MeshAttributeHandle() = default;
    MeshAttributeHandle(Mesh& m, const HandleVariant& h);
    MeshAttributeHandle(const MeshAttributeHandle& o) = default;
    MeshAttributeHandle(MeshAttributeHandle&& o) = default;
    MeshAttributeHandle& operator=(const MeshAttributeHandle& o) = default;
    MeshAttributeHandle& operator=(MeshAttributeHandle&& o) = default;

    bool operator==(const MeshAttributeHandle& o) const
    {
        return std::tie(m_mesh, m_handle) == std::tie(o.m_mesh, o.m_handle);
    }
    bool operator<(const MeshAttributeHandle& o) const
    {
        return std::tie(m_mesh, m_handle) < std::tie(o.m_mesh, o.m_handle);
    }


    // reutrns if the target mesh is the same as the one represented in the handle
    bool is_same_mesh(const Mesh&) const;


    /**
     * @brief Returns true if handle was initialized
     */
    bool is_valid() const;
    // returns if this handle still exists
    bool exists() const;

    PrimitiveType primitive_type() const;
    template <typename T>
    PrimitiveType primitive_typeT() const;
    // AttributeHandle base_handle() const ;


    template <typename T>
    auto as() const -> const held_handle_type<held_type_from_primitive<T>()>&;

    template <HeldType Type>
    auto as_from_held_type() const -> const held_handle_type<Type>&;
    // returns if the held attribute uses the primitive T


    // returns if the held attribute uses the primitive T
    template <typename T>
    bool holds() const;

    // holds basic type
    bool holds_basic_type() const;

    // returns if the held attribute uses the held type primitive Type
    template <HeldType Type>
    bool holds_from_held_type() const;

    HeldType held_type() const;

    Mesh& mesh();
    const Mesh& mesh() const;

    HandleVariant& handle() { return m_handle; }
    const HandleVariant& handle() const { return m_handle; }
    //// creates mutable accessors
    //// Implementations are in the MutableAccessor.hpp
    //// for historical reasons note that the following two classes are the same:
    //// wmtk::attribute::MutableAccessor
    //// wmtk::Accessor
    // MutableAccessor<T> create_accessor();


    template <typename T, int Dim = Eigen::Dynamic, typename MeshType = Mesh>
    const Accessor<T, MeshType, CachingAttribute<T>, Dim> create_const_accessor() const;
    template <typename T, int Dim = Eigen::Dynamic, typename MeshType = Mesh>
    Accessor<T, MeshType, CachingAttribute<T>, Dim> create_accessor() const;

    // return the dimension of the attribute (i.e the number of values stored per simplex)
    int64_t dimension() const;

    std::string name() const;


private:
    Mesh* m_mesh = nullptr;
    HandleVariant m_handle;
};

template <typename T>
inline auto MeshAttributeHandle::as() const
    -> const held_handle_type<held_type_from_primitive<T>()>&
{
    return as_from_held_type<held_type_from_primitive<T>()>();
}


template <typename T>
inline bool MeshAttributeHandle::holds() const
{
    return holds_from_held_type<held_type_from_primitive<T>()>();
}

namespace internal {
template <typename T>
struct is_typed_attribute_handle
{
    constexpr static bool value = false;
};
template <typename T>
struct is_typed_attribute_handle<TypedAttributeHandle<T>>
{
    constexpr static bool value = true;
};
} // namespace internal

template <typename T>
constexpr inline bool MeshAttributeHandle::handle_type_is_basic()
{
    return internal::is_typed_attribute_handle<T>::value;
}

inline bool MeshAttributeHandle::holds_basic_type() const
{
    return std::visit(
        [](const auto& h) noexcept -> bool {
            return handle_type_is_basic<std::decay_t<decltype(h)>>();
        },
        m_handle);
}
template <MeshAttributeHandle::HeldType Type>
inline auto MeshAttributeHandle::as_from_held_type() const -> const held_handle_type<Type>&
{
    return std::get<held_handle_type<Type>>(m_handle);
}

template <MeshAttributeHandle::HeldType Type>
inline bool MeshAttributeHandle::holds_from_held_type() const
{
    return std::holds_alternative<held_handle_type<Type>>(m_handle);
}

template <typename T>
inline constexpr auto MeshAttributeHandle::held_type_from_primitive() -> HeldType
{
    return held_type_from_handle<TypedAttributeHandle<T>>();
}
template <typename T>
inline constexpr auto MeshAttributeHandle::held_type_from_handle() -> HeldType
{
    return attribute_type_enum_from_type<typename T::Type>();
}

inline PrimitiveType MeshAttributeHandle::primitive_type() const
{
    return std::visit([](const auto& h) { return h.primitive_type(); }, m_handle);
}
template <typename T>
inline PrimitiveType MeshAttributeHandle::primitive_typeT() const
{
    return std::get<T>(m_handle).primitive_type();
}

template <typename T, int Dim, typename MeshType>
const Accessor<T, MeshType, CachingAttribute<T>, Dim> MeshAttributeHandle::create_const_accessor()
    const
{
    return Accessor<T, MeshType, CachingAttribute<T>, Dim>(mesh(), as<T>());
}
template <typename T, int Dim, typename MeshType>
Accessor<T, MeshType, CachingAttribute<T>, Dim> MeshAttributeHandle::create_accessor() const
{
    return Accessor<T, MeshType, CachingAttribute<T>, Dim>(mesh(), as<T>());
}
} // namespace wmtk::attribute
