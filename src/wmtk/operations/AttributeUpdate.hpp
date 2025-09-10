#pragma once

#include "Operation.hpp"

namespace wmtk::operations {

class AttributeUpdate : public Operation
{
public:
    AttributeUpdate(Mesh& m);

    virtual PrimitiveType primitive_type() const override { return PrimitiveType::Vertex; }
    bool attribute_new_all_configured() const final;

protected:
    virtual std::vector<simplex::Simplex> unmodified_primitives(
        const simplex::Simplex& simplex) const override;
    virtual std::vector<simplex::Simplex> execute(const simplex::Simplex& simplex) override;
};

class AttributeUpdateWithFunction : public AttributeUpdate
{
public:
    AttributeUpdateWithFunction(Mesh& m);

    using UpdateFunction = std::function<bool(Mesh&, const simplex::Simplex& s)>;

    std::vector<simplex::Simplex> execute(const simplex::Simplex& simplex) override;

    void set_function(const UpdateFunction& func) { m_function = func; }

private:
    UpdateFunction m_function;
};

} // namespace wmtk::operations
