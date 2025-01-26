#include "SimplexDart.hpp"
#include <spdlog/spdlog.h>
#include <cassert>
#include <wmtk/autogen/edge_mesh/SimplexDart.hpp>
#include <wmtk/autogen/point_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tet_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tri_mesh/SimplexDart.hpp>
#include <wmtk/utils/TupleInspector.hpp>
#include "subgroup/convert.hpp"
#include "utils/permutation_index_from_tuple.hpp"
#include "utils/simplex_index_from_permutation_index.hpp"
#include "utils/tuple_from_permutation_index.hpp"

using namespace wmtk::autogen;
namespace wmtk::dart {
SimplexDart::~SimplexDart() = default;

const SimplexDart& SimplexDart::get_singleton(wmtk::PrimitiveType simplex_type)
{
    const static point_mesh::SimplexDart pm;
    const static edge_mesh::SimplexDart em;
    const static tri_mesh::SimplexDart fm;
    const static tet_mesh::SimplexDart tm;
    switch (simplex_type) {
    case PrimitiveType::Vertex: return pm;
    case PrimitiveType::Edge: return em;
    case PrimitiveType::Triangle: return fm;
    default:
    case PrimitiveType::Tetrahedron: return tm;
    }
}
wmtk::Tuple SimplexDart::tuple_from_permutation_index(int64_t gid, int8_t index) const
{
    return utils::tuple_from_permutation_index(simplex_type(), gid, index);
}
wmtk::Tuple SimplexDart::update_tuple_from_permutation_index(const Tuple& t, int8_t index) const
{
    return utils::tuple_from_permutation_index(
        simplex_type(),
        wmtk::utils::TupleInspector::global_cid(t),
        index);
}
int8_t SimplexDart::permutation_index_from_tuple(const wmtk::Tuple& t) const
{
    auto r = utils::permutation_index_from_tuple(simplex_type(), t);
    spdlog::info("{}", r);
    return r;

}

int8_t SimplexDart::convert(int8_t permutation_index, const SimplexDart& target) const
{
    if (target.simplex_type() == PrimitiveType::Vertex) {
        return 0;
    } else if (simplex_type() == PrimitiveType::Vertex) {
        return target.identity();
    } else {
        return subgroup::convert(simplex_type(), target.simplex_type(), permutation_index);
    }
}

wmtk::Tuple SimplexDart::tuple_from_dart(const Dart& dart) const
{
    return tuple_from_permutation_index(dart.global_id(), dart.permutation());
}
Dart SimplexDart::dart_from_tuple(const wmtk::Tuple& t) const
{
    return Dart{
        wmtk::utils::TupleInspector::global_cid(t),
        wmtk::dart::utils::permutation_index_from_tuple(simplex_type(), t)};
}

int8_t SimplexDart::simplex_index(const Dart& dart, PrimitiveType simplex_type) const
{
    return simplex_index(dart.permutation(), simplex_type);
}
int8_t SimplexDart::simplex_index(const int8_t permutation_index, PrimitiveType simplex_type) const
{
    return utils::simplex_index_from_permutation_index(
        this->simplex_type(),
        permutation_index,
        simplex_type);
}
Dart SimplexDart::act(const Dart& d, int8_t action) const
{
    return Dart(d.global_id(), product(action, d.permutation()));
}

} // namespace wmtk::dart
