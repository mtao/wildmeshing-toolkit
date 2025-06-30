#include "SimplexDart.hpp"
#include <spdlog/spdlog.h>
#include <cassert>
#include <wmtk/autogen/edge_mesh/SimplexDart.hpp>
#include <wmtk/autogen/point_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tet_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tri_mesh/SimplexDart.hpp>
#include "subgroup/convert.hpp"
#include "utils/permutation_index_from_tuple.hpp"
#include "utils/simplex_index_from_permutation_index.hpp"
#include "utils/tuple_from_permutation_index.hpp"

using namespace wmtk::autogen;
namespace wmtk::dart {
namespace {
const static point_mesh::SimplexDart psd_singleton;
const static edge_mesh::SimplexDart esd_singleton;
const static tri_mesh::SimplexDart fsd_singleton;
const static tet_mesh::SimplexDart tsd_singleton;
} // namespace
SimplexDart::~SimplexDart() = default;

const SimplexDart& SimplexDart::get_singleton(wmtk::PrimitiveType simplex_type)
{
    switch (simplex_type) {
    case PrimitiveType::Vertex: return psd_singleton;
    case PrimitiveType::Edge: return esd_singleton;
    case PrimitiveType::Triangle: return fsd_singleton;
    default:
    case PrimitiveType::Tetrahedron: return tsd_singleton;
    }
}
const SimplexDart& SimplexDart::get_singleton(int8_t simplex_type)
{
    switch (simplex_type) {
    case 0: return psd_singleton;
    case 1: return esd_singleton;
    case 2: return fsd_singleton;
    default:
    case 3: return tsd_singleton;
    }
}
wmtk::Tuple SimplexDart::tuple_from_permutation_index(int64_t gid, int8_t index) const
{
    return utils::tuple_from_permutation_index(simplex_type(), gid, index);
}
wmtk::Tuple SimplexDart::update_tuple_from_permutation_index(const Tuple& t, int8_t index) const
{
    return utils::tuple_from_permutation_index(simplex_type(), t.global_cid(), index);
}
int8_t SimplexDart::permutation_index_from_tuple(const wmtk::Tuple& t) const
{
    auto r = utils::permutation_index_from_tuple(simplex_type(), t);
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
    if (dart.is_null()) {
        return {};
    } else {
        return tuple_from_permutation_index(dart.global_id(), dart.permutation());
    }
}
Dart SimplexDart::dart_from_tuple(const wmtk::Tuple& t) const
{
    if (t.is_null()) {
        return {};
    } else {
        return Dart{
            t.global_cid(),
            wmtk::dart::utils::permutation_index_from_tuple(simplex_type(), t)};
    }
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
int8_t SimplexDart::act(int8_t permutation, int8_t action) const
{
    return product(permutation, action);
}

int8_t SimplexDart::product(const std::initializer_list<int8_t>& indices) const
{
    auto it = indices.begin();
    int8_t prod = *it;
    ++it;
    for (; it != indices.end(); ++it) {
        prod = product(prod, *it);
    }
    return prod;
}
Dart SimplexDart::act(const Dart& d, int8_t action) const
{
    return Dart(d.global_id(), act(d.permutation(), action));
}


bool SimplexDart::is_valid(int8_t index) const
{
    return index >= 0 && index < size();
}
bool SimplexDart::is_valid(const Dart& d) const
{
    return is_valid(d.permutation());
}
} // namespace wmtk::dart
