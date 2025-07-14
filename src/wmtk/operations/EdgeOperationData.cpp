
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

void EdgeOperationData::set_split(const Mesh& m, const Tuple& t)
{
    m_op_data = std::make_unique<internal::SplitAlternateFacetData>(m, t);
}
void EdgeOperationData::set_collapse(const Mesh& m, const Tuple& t)
{
    m_op_data = std::make_unique<internal::CollapseAlternateFacetData>(m, t);
}

void EdgeOperationData::set_split(const Mesh& m, const Tuple& t, const std::vector<Tuple>& ts)
{
    m_op_data = std::make_unique<internal::CollapseAlternateFacetData>(m, t, ts);
}
void EdgeOperationData::set_collapse(const Mesh& m, const Tuple& t, const std::vector<Tuple>& ts)
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
    // wmtk::logger()
    //     .trace("Requesting {} {}-simplices. got [{}]", count, int(type), fmt::join(ret, ","));
    return ret;
}
Tuple EdgeOperationData::get_alternative(const PrimitiveType mesh_pt, const Tuple& t) const
{
    return std::visit([&](const auto& m) { return m->get_alternative(mesh_pt, t); }, m_op_data);
}

void EdgeOperationData::delete_simplices()
{
    for (size_t d = 0; d < simplex_ids_to_delete.size(); ++d) {
        //wmtk::logger().info(
        //    "{}-Mesh-Deleting {} {}-simplices [{}]",
        //    m_mesh.top_simplex_type(),
        //    simplex_ids_to_delete[d].size(),
        //    d,
        //    fmt::join(simplex_ids_to_delete[d], ","));
        for (const int64_t id : simplex_ids_to_delete[d]) {
            m_mesh.get_flag_accessor(get_primitive_type_from_id(d)).index_access().deactivate(id);
        }
    }
}

} // namespace wmtk::operations
