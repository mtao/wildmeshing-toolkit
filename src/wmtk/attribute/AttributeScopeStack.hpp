#pragma once
#include <spdlog/spdlog.h>
#include <memory>
#include <vector>
#include "AttributeCache.hpp"
#include "AttributeHandle.hpp"

namespace wmtk {
class Mesh;
namespace attribute {
template <typename T>
class Attribute;
template <typename T>
class AccessorBase;
template <typename T>
class AttributeScope;
class AttributeManager;

/**
 * A stack of changes applied to an Attribute.
 * The stack consists of AttributeScopes which hold all changes applied inside one scope. Whenever a
 * new scope is created, it is pushed to the stack. As soon as an AttributeScopeHandle is
 * destructed, `push_scope` of all Attributes is triggered.
 */
template <typename T>
class AttributeScopeStack
{
public:
    using MapResult = typename AttributeCache<T>::MapResult;
    using ConstMapResult = typename AttributeCache<T>::ConstMapResult;
    // stack is implemented by a parent pointing graph, so we track a pointer
    // to the leaf
    AttributeScopeStack();
    ~AttributeScopeStack();
    void emplace();
    void pop(Attribute<T>& attribute, bool apply_updates);
    AttributeScope<T>* active_scope_ptr();
    const AttributeScope<T>* active_scope_ptr() const;

    bool empty() const;
    void clear_current_scope(Attribute<T>& attr);

    int64_t depth() const;
#if defined(WMTK_ENABLE_GENERIC_CHECKPOINTS)
    int64_t add_checkpoint();
    AttributeScope<T> const* get_checkpoint(int64_t index) const;
#endif

    // go to the next most historic scope
    void change_to_next_scope() const;
    void change_to_previous_scope() const;
    // go to the scope with active data
    void change_to_current_scope() const;

    bool at_current_scope() const;
    bool writing_enabled() const;

    void flush_changes_to_vector(const Attribute<T>& attr, std::vector<T>& data) const;

#if defined(WMTK_FLUSH_ON_FAIL)
    MapResult vector_attribute(AccessorBase<T>& accessor, int64_t index);

    ConstMapResult const_vector_attribute(const AccessorBase<T>& accessor, int64_t index) const;


    T& scalar_attribute(AccessorBase<T>& accessor, int64_t index);

    T const_scalar_attribute(const AccessorBase<T>& accessor, int64_t index) const;
    T const_scalar_attribute(const AccessorBase<T>& accessor, int64_t index, int8_t offset) const;
#endif

protected:
    std::unique_ptr<AttributeScope<T>> m_start;
    mutable AttributeScope<T>* m_active = nullptr;
#if defined(WMTK_ENABLE_GENERIC_CHECKPOINTS)
    std::vector<AttributeScope<T> const*> m_checkpoints;
#endif
    // Mesh& m_mesh;
    // AttributeManager& m_attribute_manager;
    // MeshAttributeHandle<T> m_handle;
};

template <typename T>
auto AttributeScopeStack<T>::vector_attribute(AccessorBase<T>& accessor, int64_t index) -> MapResult
{
    assert(writing_enabled());


#if defined(WMTK_FLUSH_ON_FAIL)
    // make sure we record the original value of this attribute by inserting if it hasn't been
    // inserted yet
    auto value = accessor.vector_attribute(index);
    if (bool(m_start)) {
        spdlog::info("Adress of l: {} of {}", fmt::ptr(&m_start->m_data), fmt::ptr(m_start.get()));
        spdlog::info("{}", m_start->m_data.size());

        spdlog::info("Tried to size!");
        auto it = m_start->m_data.find(index);
        spdlog::info("Tried to find!");
        if(it == m_start->m_data.end()) {

            spdlog::info("FAIL!");
            m_start->m_data[index].data = value;
            spdlog::info("FAIL.");
        }
        //auto [it, was_inserted] = l.try_emplace(index,AttributeCacheData<T>{});
        //spdlog::info("Finished a try_emplace and got {}", was_inserted);
        //if (was_inserted) {
        //    it->second.data = value;
        //}
        spdlog::info("Got past assignment");
    }

    return value;
#else
    if (m_active) {
        return m_active->vector_attribute(accessor, index);
    } else {
        return accessor.vector_attribute(index);
    }

#endif
}

template <typename T>
inline auto AttributeScopeStack<T>::const_vector_attribute(
    const AccessorBase<T>& accessor,
    int64_t index) const -> ConstMapResult
{
    if (m_active != nullptr) {
        return m_active->const_vector_attribute(accessor, index);
    } else {
        return accessor.const_vector_attribute(index);
    }
}

template <typename T>
inline auto AttributeScopeStack<T>::scalar_attribute(AccessorBase<T>& accessor, int64_t index) -> T&
{
    return vector_attribute(accessor, index)(0);
}

template <typename T>
inline auto AttributeScopeStack<T>::const_scalar_attribute(
    const AccessorBase<T>& accessor,
    int64_t index) const -> T
{
    return const_vector_attribute(accessor, index)(0);
}
template <typename T>
inline auto AttributeScopeStack<T>::const_scalar_attribute(
    const AccessorBase<T>& accessor,
    int64_t index,
    int8_t offset) const -> T
{
    if (m_active != nullptr) {
        return m_active->const_vector_attribute(accessor, index)(offset);
    } else {
        return accessor.const_scalar_attribute(index, offset);
    }
}
} // namespace attribute
} // namespace wmtk
