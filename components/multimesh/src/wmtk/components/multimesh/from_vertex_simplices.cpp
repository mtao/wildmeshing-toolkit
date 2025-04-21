
#include "from_vertex_simplices.hpp"

#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/utils/DisjointSet.hpp>

#include <wmtk/Types.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/mesh_utils.hpp>

#include <wmtk/Types.hpp>
#include <wmtk/dart/SimplexDart.hpp>
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

template <size_t Dim, typename MeshType>
std::vector<std::array<Tuple, 2>> from_vertex_simplices_map_(
    MeshType& parent,
    const wmtk::utils::internal::IndexSimplexMapper& indexer,
    const MatrixXl& l)
{
    using VecType = wmtk::Vector<int64_t, Dim>;
    using MapType = VecType::MapType;

    const auto& sd = wmtk::dart::SimplexDart::get_singleton(parent.top_simplex_type());
    std::vector<std::array<Tuple, 2>> R;
    R.reserve(l.rows());
    for (int j = 0; j < l.rows(); ++j) {
        std::array<int64_t, Dim> r;
        MapType(r.data()) = l.row(j);
        R.emplace_back(std::array<Tuple, 2>{
            {sd.tuple_from_dart(wmtk::dart::Dart(j, sd.identity())), indexer.get_tuple(r)}});
    }
    return R;
}
std::vector<std::array<Tuple, 2>> from_vertex_simplices_map(
    Mesh& parent,
    const wmtk::utils::internal::IndexSimplexMapper& indexer,
    const MatrixXl& l)
{
    switch (l.cols()) {
    default:
    case 0: return {};
    case 1:
        return from_vertex_simplices_map_<1, PointMesh>(
            static_cast<PointMesh&>(parent),
            indexer,
            l);
    case 2:
        return from_vertex_simplices_map_<2, EdgeMesh>(static_cast<EdgeMesh&>(parent), indexer, l);
    case 3:
        return from_vertex_simplices_map_<3, TriMesh>(static_cast<TriMesh&>(parent), indexer, l);
    case 4:
        return from_vertex_simplices_map_<4, TetMesh>(static_cast<TetMesh&>(parent), indexer, l);
    }
}

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
        int64_t a = indexer.size();
        indexer.try_emplace(j, a);
    }
    l2 = l.unaryExpr([&](int64_t a) { return indexer.at(a); });
    const auto pt = get_primitive_type_from_id(l.cols() - 1);

    std::shared_ptr<Mesh> child;
    switch (pt) {
    default:
    case PrimitiveType::Vertex: break;
    case PrimitiveType::Edge: {
        auto m = std::make_shared<EdgeMesh>();
        m->initialize(l2);
        return m;
    }
    case PrimitiveType::Triangle: {
        auto m = std::make_shared<TriMesh>();
        m->initialize(l2);
        return m;
    }
    case PrimitiveType::Tetrahedron: {
        auto m = std::make_shared<TetMesh>();
        m->initialize(l2);
        return m;
    }
    }

    parent.register_child_mesh(child, child_map);


    return child;
}

} // namespace wmtk::components::multimesh
