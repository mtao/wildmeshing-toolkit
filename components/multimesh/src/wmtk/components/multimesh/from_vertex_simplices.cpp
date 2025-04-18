
#include "from_vertex_simplices.hpp"

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/utils/DisjointSet.hpp>

#include <wmtk/Types.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/mesh_utils.hpp>

#include <wmtk/Types.hpp>
#include "from_facet_bijection.hpp"
#include "wmtk/utils/internal/IndexSimplexMapper.hpp"


namespace wmtk::components::multimesh {
/*
std::shared_ptr<Mesh>
from_vertex_simplices(EdgeMesh& parent, const MatrixXl& l, const std::string_view& name)
{
wmtk::utils::EigenMatrixWriter writer;
parent.serialize(writer);
MatrixXl S = writer.get_simplex_vertex_matrix();
// auto h = parent.get_attribute_handle<int64_t>("m_ev_handle", wmtk::PrimitiveType::Edge);
// assert(h.is_valid());
return nullptr;
}
std::shared_ptr<Mesh>
from_vertex_simplices(TriMesh& parent, const MatrixXl& l, const std::string_view& name)
{
wmtk::utils::EigenMatrixWriter writer;
parent.serialize(writer);
MatrixXl S = writer.get_simplex_vertex_matrix();
// auto h = parent.get_attribute_handle<int64_t>("m_fv_handle", wmtk::PrimitiveType::Triangle);
// assert(h.is_valid());
return nullptr;
}
std::shared_ptr<Mesh>
from_vertex_simplices(TetMesh& parent, const MatrixXl& l, const std::string_view& name)
{
wmtk::utils::EigenMatrixWriter writer;
parent.serialize(writer);
MatrixXl S = writer.get_simplex_vertex_matrix();
// auto h = parent.get_attribute_handle<int64_t>("m_tv_handle",
// wmtk::PrimitiveType::Tetrahedron); assert(h.is_valid());
return nullptr;
}
*/
std::vector<std::array<Tuple, 2>> from_vertex_simplices_map(
    Mesh& parent,
    const wmtk::utils::internal::IndexSimplexMapper& indexer,
    const MatrixXl& l)
{}

std::vector<std::array<Tuple, 2>> from_vertex_simplices_map(Mesh& parent, const MatrixXl& l)
{
    return from_vertex_simplices_map(parent, wmtk::utils::internal::IndexSimplexMapper(parent), l);
}
std::shared_ptr<Mesh> from_vertex_simplices(Mesh& parent, const MatrixXl& l)
{
    auto child_map = from_vertex_simplices_map(parent, l);

    MatrixXl l2;
    std::map<int64_t, int64_t> indexer;
    for (const auto& j : std::span(l.data(), l.data() + l.size())) {
        indexer[j] = indexer.size();
    }

    std::shared_ptr<Mesh> child;
    switch (parent.top_simplex_type()) {
    default:
    case PrimitiveType::Vertex: break;
    case PrimitiveType::Edge: return std::make_shared<EdgeMesh>(l2);
    case PrimitiveType::Triangle: return std::make_shared<TriMesh>(l2);
    case PrimitiveType::Tetrahedron: return std::make_shared<TetMesh>(l2);
    }

    parent.register_child_mesh(child, child_map);


    return child;
}

} // namespace wmtk::components::multimesh
