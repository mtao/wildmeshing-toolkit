#include "Attribute.hpp"


namespace wmtk::attribute {

template <typename T>
class PerThreadAttributeScopeStacks;
template <typename T>
class AttributeScopeStack;


template <typename T>
class CachingAttribute: public Attribute<T> {
    public:
    void push_scope();
    void pop_scope(bool apply_updates);
    void clear_current_scope();

    // returns nullptr if no scope exists
    AttributeScopeStack<T>* get_local_scope_stack_ptr();
    private:

    std::unique_ptr<PerThreadAttributeScopeStacks<T>> m_scope_stacks;
};
}
