#pragma once
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include "Invariant.hpp"

namespace wmtk::invariants {

template <typename T>
class AttributeInvariant : public Invariant
{
public:
    AttributeInvariant(
        const attribute::MeshAttributeHandle& handle,
        bool use_before = true,
        bool use_old_state_in_after = true,
        bool use_new_state_in_after = true);
    AttributeInvariant(
        const Mesh& m,
        const attribute::TypedAttributeHandle<T>& handle,
        bool use_before = true,
        bool use_old_state_in_after = true,
        bool use_new_state_in_after = true);
    const attribute::TypedAttributeHandle<T>& handle() { return m_handle; }

private:
    attribute::TypedAttributeHandle<T> m_handle;
};

template <typename T>
AttributeInvariant<T>::AttributeInvariant(
    const attribute::MeshAttributeHandle& handle,
    bool use_before,
    bool use_old_state_in_after,
    bool use_new_state_in_after)
    : Invariant(handle.mesh(), use_before, use_old_state_in_after, use_new_state_in_after)
    , m_handle(handle.as<T>())
{}
template <typename T>
AttributeInvariant<T>::AttributeInvariant(
    const Mesh& m,
    const attribute::TypedAttributeHandle<T>& handle,
    bool use_before,
    bool use_old_state_in_after,
    bool use_new_state_in_after)
    : Invariant(m, use_before, use_old_state_in_after, use_new_state_in_after)
    , m_handle(handle)
{}
} // namespace wmtk::invariants
