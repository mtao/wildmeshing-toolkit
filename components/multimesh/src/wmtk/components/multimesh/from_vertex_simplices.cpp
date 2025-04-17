
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


namespace wmtk::components::multimesh {
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

std::vector<std::pair<Tuple, Tuple>>
from_vertex_simplices_map(Mesh& parent, const MatrixXl& l, const std::string_view& name)
{
    return {};
}
std::shared_ptr<Mesh>
from_vertex_simplices(Mesh& parent, const MatrixXl& l, const std::string_view& name)
{
    wmtk::utils::EigenMatrixWriter writer;
    parent.serialize(writer);
    MatrixXl S = writer.get_simplex_vertex_matrix();

    /*
    switch(parent.top_simplex_type()) {
        default:
    case PrimitiveType::Vertex: break;
    case PrimitiveType::Edge:
                                return from_vertex_simplices(static_cast<EdgeMesh&>(parent),l,name);
    case PrimitiveType::Triangle:
                                return from_vertex_simplices(static_cast<TriMesh&>(parent),l,name);
    case PrimitiveType::Tetrahedron:
                                return from_vertex_simplices(static_cast<TetMesh&>(parent),l,name);
    }
    */

    return nullptr;
}

} // namespace wmtk::components::multimesh
