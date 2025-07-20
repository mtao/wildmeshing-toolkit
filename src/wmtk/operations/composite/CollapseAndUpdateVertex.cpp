#include "CollapseAndUpdateVertex.hpp"
#include <wmtk/multimesh/MultiMeshVisitor.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>

namespace wmtk::operations::composite {
CollapseAndUpdateVertex::CollapseAndUpdateVertex(Mesh& m)
    : CollapseAndUpdateVertex(m, std::make_shared<EdgeCollapse>(m), {})
{}
CollapseAndUpdateVertex::CollapseAndUpdateVertex(
    Mesh& m,
    std::shared_ptr<EdgeCollapse> collapse,
    std::shared_ptr<Operation> op)
    : Operation(m)
    , m_collapse(std::move(collapse))
    , m_vertex_update(std::move(op))
{
    if (bool(m_vertex_update)) {
        assert(m_vertex_update->primitive_type() == PrimitiveType::Vertex);
    }
}

void CollapseAndUpdateVertex::set_vertex_update(std::shared_ptr<Operation> op)
{
    assert(op->primitive_type() == PrimitiveType::Vertex);
    m_vertex_update = op;
}

std::vector<simplex::Simplex> CollapseAndUpdateVertex::execute(const simplex::Simplex& simplex)
{
    assert(simplex.primitive_type() == PrimitiveType::Edge);
    assert(bool(m_vertex_update));
    //assert(wmtk::multimesh::utils::check_maps_valid(mesh()));
    // switch edge - switch face
    //    /|\ .
    //   / ^ \ .
    //  /f |  \ .
    //  ---X---
    //  \  |  /
    //   \ | /
    //    \|/
    const auto collapse_simplices = collapse()(simplex);
    if (collapse_simplices.empty()) return {};
    assert(collapse_simplices.size() == 1);

    //assert(wmtk::multimesh::utils::check_maps_valid(mesh()));
    simplex::Simplex s = simplex::Simplex::vertex(collapse_simplices[0].tuple());
    assert(mesh().is_valid(s));

    return vertex_update()(s);
}


std::vector<simplex::Simplex> CollapseAndUpdateVertex::unmodified_primitives(
    const simplex::Simplex& simplex) const
{
    return {simplex};
}
} // namespace wmtk::operations::composite
