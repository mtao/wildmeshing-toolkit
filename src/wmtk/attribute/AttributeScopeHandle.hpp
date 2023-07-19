#pragma once

namespace wmtk {
class AttributeManager;
class AttributeScopeHandle
{
public:
    AttributeScopeHandle(AttributeManager& manager);
    ~AttributeScopeHandle();


    void mark_failed();

private:
    AttributeManager& m_manager;
    bool m_failed = false;
};
} // namespace wmtk