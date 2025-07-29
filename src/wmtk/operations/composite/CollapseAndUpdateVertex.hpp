#pragma once
#include <wmtk/operations/Operation.hpp>

namespace wmtk::operations {

    class EdgeCollapse;
}
namespace wmtk::operations::composite {
class CollapseAndUpdateVertex : public Operation
{
public:
    CollapseAndUpdateVertex(Mesh& m);
    CollapseAndUpdateVertex(
        Mesh& m,
        std::shared_ptr<EdgeCollapse> collapse,
        std::shared_ptr<Operation> vertex_op = {});

    PrimitiveType primitive_type() const override { return PrimitiveType::Edge; }
    EdgeCollapse& collapse() { return *m_collapse; }
    Operation& vertex_update() { return *m_vertex_update; }
    const EdgeCollapse& collapse() const { return *m_collapse; }
    const Operation& vertex_update() const { return *m_vertex_update; }

    void set_vertex_update(std::shared_ptr<Operation> op);
    std::vector<simplex::Simplex> unmodified_primitives(
        const simplex::Simplex& simplex) const override;
    std::vector<simplex::Simplex> execute(const simplex::Simplex& simplex) override;
    bool attribute_new_all_configured() const final;


protected:
    std::shared_ptr<EdgeCollapse> m_collapse;
    std::shared_ptr<Operation> m_vertex_update;
};
} // namespace wmtk::operations::composite

