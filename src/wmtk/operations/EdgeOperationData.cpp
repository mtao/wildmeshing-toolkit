
#include <array>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/Mesh.hpp>
#include "EdgeOperationData.hpp"
#include "internal/CollapseAlternateFacetData.hpp"
#include "internal/SplitAlternateFacetData.hpp"
namespace wmtk::operations {

EdgeOperationData::EdgeOperationData(Mesh& m, const Tuple& operating_tuple)
    : m_mesh(m)
    , m_operating_tuple(operating_tuple)
    , m_input_edge_gid(m_mesh.id(m_operating_tuple, PrimitiveType::Edge))
    , m_spine_vids(
          std::array<int64_t, 2>{
              {m_mesh.id(m_operating_tuple, PrimitiveType::Vertex),
               m_mesh.id(
                   m_mesh.switch_tuple(m_operating_tuple, PrimitiveType::Vertex),
                   PrimitiveType::Vertex)}})

{}
EdgeOperationData::~EdgeOperationData() = default;
EdgeOperationData::EdgeOperationData(EdgeOperationData&&) = default;
auto EdgeOperationData::tuple_from_id(const Mesh& m, const PrimitiveType type, const int64_t gid)
    -> Tuple
{
    return m.tuple_from_id(type, gid);
}
simplex::Simplex
EdgeOperationData::simplex_from_id(const Mesh& m, const PrimitiveType type, const int64_t gid)
{
    return m.simplex_from_id(type, gid);
}
void EdgeOperationData::set_split()
{
    m_op_data = std::make_unique<internal::SplitAlternateFacetData>();
}
void EdgeOperationData::set_collapse()
{
    m_op_data = std::make_unique<internal::CollapseAlternateFacetData>();
}

void EdgeOperationData::set_split(Mesh& m, const Tuple& t)
{
    m_op_data = std::make_unique<internal::SplitAlternateFacetData>(m, t);
}
void EdgeOperationData::set_collapse(Mesh& m, const Tuple& t)
{
    m_op_data = std::make_unique<internal::CollapseAlternateFacetData>(m, t);
}

void EdgeOperationData::set_split(Mesh& m, const Tuple& t, const std::vector<Tuple>& ts)
{
    m_op_data = std::make_unique<internal::CollapseAlternateFacetData>(m, t, ts);
}
void EdgeOperationData::set_collapse(Mesh& m, const Tuple& t, const std::vector<Tuple>& ts)
{
    m_op_data = std::make_unique<internal::CollapseAlternateFacetData>(m, t, ts);
}

const internal::SplitAlternateFacetData& EdgeOperationData::const_split_facet_data() const
{
    const auto& ptr = std::get<std::unique_ptr<internal::SplitAlternateFacetData>>(m_op_data);
    if (!bool(ptr)) {
        throw std::runtime_error(
            "Split alternate facet data does not exist, ptr in variant was null");
    }
    return *ptr;
}
const internal::CollapseAlternateFacetData& EdgeOperationData::const_collapse_facet_data() const
{
    const auto& ptr = std::get<std::unique_ptr<internal::CollapseAlternateFacetData>>(m_op_data);
    if (!bool(ptr)) {
        throw std::runtime_error(
            "Collapse alternate facet data does not exist, ptr in variant was null");
    }
    return *ptr;
}
internal::SplitAlternateFacetData& EdgeOperationData::split_facet_data()
{
    auto& ptr = std::get<std::unique_ptr<internal::SplitAlternateFacetData>>(m_op_data);
    if (!bool(ptr)) {
        throw std::runtime_error(
            "Split alternate facet data does not exist, ptr in variant was null");
    }
    return *ptr;
}
internal::CollapseAlternateFacetData& EdgeOperationData::collapse_facet_data()
{
    auto& ptr = std::get<std::unique_ptr<internal::CollapseAlternateFacetData>>(m_op_data);
    if (!bool(ptr)) {
        throw std::runtime_error(
            "Collapse alternate facet data does not exist, ptr in variant was null");
    }
    return *ptr;
}
std::vector<int64_t>
EdgeOperationData::request_simplex_indices(Mesh& mesh, const PrimitiveType type, int64_t count)
{
    mesh.guarantee_more_attributes(type, count);

    auto ret = mesh.request_simplex_indices(type, count);
#if !defined(NDEBUG)
     wmtk::logger()
         .trace("{}-Mesh requesting {} {}-simplices. got [{}]", mesh.top_cell_dimension(), count, int(type), fmt::join(ret, ","));
#endif
    return ret;
}
std::vector<int64_t>
EdgeOperationData::request_simplex_indices(const PrimitiveType type, int64_t count)
{
    return request_simplex_indices(m_mesh, type, count);
}
Tuple EdgeOperationData::get_alternative(const PrimitiveType mesh_pt, const Tuple& t) const
{
    return std::visit([&](const auto& m) { return m->get_alternative(mesh_pt, t); }, m_op_data);
}

std::vector<int64_t> EdgeOperationData::get_simplex_ids_to_delete(const PrimitiveType pt) const
{
    return std::visit(
        [&](const auto& m) { return m->get_simplices_to_delete(m_mesh, pt); },
        m_op_data);
}
void EdgeOperationData::set_simplex_ids_to_delete()
{
    for (int8_t j = 0; j <= m_mesh.top_cell_dimension(); ++j) {
        simplex_ids_to_delete[j] = get_simplex_ids_to_delete(get_primitive_type_from_id(j));
    }
}

void EdgeOperationData::delete_simplices()
{
    for (size_t d = 0; d < simplex_ids_to_delete.size(); ++d) {
#if !defined(NDEBUG)
         wmtk::logger().trace(
             "{}-Mesh-Deleting {} {}-simplices [{}]",
             m_mesh.top_cell_dimension(),
             simplex_ids_to_delete[d].size(),
             d,
             fmt::join(simplex_ids_to_delete[d], ","));
#endif
        for (const int64_t id : simplex_ids_to_delete[d]) {
            m_mesh.get_flag_accessor(get_primitive_type_from_id(d)).index_access().deactivate(id);
        }
    }
}

} // namespace wmtk::operations
