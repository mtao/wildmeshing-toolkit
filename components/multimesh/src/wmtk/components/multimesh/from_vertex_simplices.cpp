
#include "from_vertex_simplices.hpp"

#include <wmtk/Mesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/utils/DisjointSet.hpp>

#include <wmtk/Types.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/mesh_utils.hpp>

#include "from_facet_bijection.hpp"
#include <wmtk/Types.hpp>


namespace wmtk::components::multimesh {
std::shared_ptr<Mesh> from_vertex_simplices(EdgeMesh& parent, const MatrixXl& l, const std::string_view& name) {

    wmtk::utils::EigenMatrixWriter writer;
    parent.serialize(writer);
    MatrixXl S = writer.get_simplex_vertex_matrix();
    //auto h = parent.get_attribute_handle<int64_t>("m_ev_handle", wmtk::PrimitiveType::Edge);
    //assert(h.is_valid());
    return nullptr;

}
std::shared_ptr<Mesh> from_vertex_simplices(TriMesh& parent, const MatrixXl& l, const std::string_view& name) {
    wmtk::utils::EigenMatrixWriter writer;
    parent.serialize(writer);
    MatrixXl S = writer.get_simplex_vertex_matrix();
    //auto h = parent.get_attribute_handle<int64_t>("m_fv_handle", wmtk::PrimitiveType::Triangle);
    //assert(h.is_valid());
    return nullptr;

}
std::shared_ptr<Mesh> from_vertex_simplices(TetMesh& parent, const MatrixXl& l, const std::string_view& name) {
    wmtk::utils::EigenMatrixWriter writer;
    parent.serialize(writer);
    MatrixXl S = writer.get_simplex_vertex_matrix();
    //auto h = parent.get_attribute_handle<int64_t>("m_tv_handle", wmtk::PrimitiveType::Tetrahedron);
    //assert(h.is_valid());
    return nullptr;

}
MatrixXl concatenate(const std::vector<MatrixXl>& container) {

    Eigen::Index rows= 0;
    Eigen::Index cols = -1;
    for(const auto& m: container) {
        rows += m.rows();
        if(cols == -1) {
            cols = m.cols();
        } else {
            assert(cols == m.cols());
        }
    }
    MatrixXl R(rows,cols); 
    Eigen::Index offset = 0;
    Eigen::Index voffset = 0;
    for(const auto& m: container) {
        auto b = R.block(offset,0, m.rows(), m.cols());
        b = m.array() + voffset;
        voffset = b.maxCoeff() + 1;
    }
    return R;
}

std::shared_ptr<Mesh> from_vertex_simplices(Mesh& parent, const MatrixXl& l, const std::string_view& name) {
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
