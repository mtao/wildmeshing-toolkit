#include "SimplexDart.hpp"
#include <cassert>
#include <wmtk/autogen/edge_mesh/SimplexDart.hpp>
#include <wmtk/autogen/point_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tet_mesh/SimplexDart.hpp>
#include <wmtk/autogen/tri_mesh/SimplexDart.hpp>
#include <wmtk/utils/TupleInspector.hpp>
#include "utils/simplex_index_from_permutation_index.hpp"
#include "subgroup/convert.hpp"
#include "utils/tuple_from_permutation_index.hpp"
#include "utils/permutation_index_from_tuple.hpp"

using namespace wmtk::autogen;
namespace wmtk::dart {
namespace {


/*
int8_t empty_binary(int8_t,int8_t){
    assert(false);
}

int8_t empty_unary(int8_t){
    assert(false);
}
int8_t empty_unary(){
    assert(false);
}

template <typename T>
    void f();

template <typename
    */


/*
#define GET_OP(NAME, RETTYPE)                                    \
    auto get_##NAME(PrimitiveType pt)->SimplexDart::RETTYPE      \
    {                                                            \
        switch (pt) {                                            \
        case PrimitiveType::Edge: return &edge_mesh::NAME;       \
        case PrimitiveType::Triangle: return &tri_mesh::NAME;    \
        case PrimitiveType::Tetrahedron: return &tet_mesh::NAME; \
        case PrimitiveType::Vertex: return &point_mesh::NAME;    \
        default: assert(false);                                  \
        }                                                        \
        return nullptr;                                          \
    }
GET_OP(product, binary_op_type)
GET_OP(inverse, unary_op_type)
GET_OP(primitive_to_index, primitive_to_index_type)
GET_OP(identity, nullary_op_type)
GET_OP(opposite, nullary_op_type)
} // namespace

#define FORWARD_OP(NAME, OP, RETTYPE, DEFAULT)                  \
    auto SimplexDart::NAME() const -> RETTYPE                   \
    {                                                           \
        switch (m_simplex_type) {                               \
        case PrimitiveType::Edge: return edge_mesh::OP();       \
        case PrimitiveType::Triangle: return tri_mesh::OP();    \
        case PrimitiveType::Tetrahedron: return tet_mesh::OP(); \
        case PrimitiveType::Vertex: return point_mesh::OP();    \
        default: assert(false);                                 \
        }                                                       \
        return DEFAULT;                                         \
    }

FORWARD_OP(size, size, size_t, {})
using DynamicIntMap = VectorX<int8_t>::ConstMapType;
namespace {
const static DynamicIntMap nullmap = DynamicIntMap(nullptr, 0);
}
FORWARD_OP(permutation_indices, permutation_indices_dynamic, DynamicIntMap, nullmap)

SimplexDart::SimplexDart(wmtk::PrimitiveType simplex_type)
    : m_simplex_type(simplex_type)
    , m_product(get_product(simplex_type))
    , m_inverse(get_inverse(simplex_type))
    , m_primitive_to_index(get_primitive_to_index(simplex_type))
    , m_identity(get_identity(simplex_type))
    , m_opposite(get_opposite(simplex_type))
{}
const SimplexDart& SimplexDart::get_singleton(wmtk::PrimitiveType simplex_type)
{
    const static std::array<SimplexDart, 4> singletons = {
        {SimplexDart(PrimitiveType::Vertex),
         SimplexDart(PrimitiveType::Edge),
         SimplexDart(PrimitiveType::Triangle),
         SimplexDart(PrimitiveType::Tetrahedron)}};
    return singletons[get_primitive_type_id(simplex_type)];
}

int8_t SimplexDart::product(int8_t a, int8_t b) const
{
    return m_product(a, b);
}
int8_t SimplexDart::inverse(int8_t a) const
{
    return m_inverse(a);
}
Dart SimplexDart::act(const Dart& d, int8_t action) const
{
    return Dart(d.global_id(), product(action, d.permutation()));
}

int8_t SimplexDart::primitive_as_index(wmtk::PrimitiveType pt) const
{
    return m_primitive_to_index(pt);
}
int8_t SimplexDart::identity() const
{
    return m_identity();
}
int8_t SimplexDart::opposite() const
{
    return m_opposite();
}
wmtk::Tuple SimplexDart::tuple_from_permutation_index(int64_t gid, int8_t index) const
{
    return utils::tuple_from_permutation_index(m_simplex_type, gid, index);
}
wmtk::Tuple SimplexDart::update_tuple_from_permutation_index(const Tuple& t, int8_t index) const
{
    return utils::tuple_from_permutation_index(
        m_simplex_type,
        wmtk::utils::TupleInspector::global_cid(t),
        index);
}
int8_t SimplexDart::permutation_index_from_tuple(const wmtk::Tuple& t) const
{
    return utils::permutation_index_from_tuple(m_simplex_type, t);
}

int8_t SimplexDart::convert(int8_t permutation_index, const SimplexDart& target) const
{
    if (target.m_simplex_type == PrimitiveType::Vertex) {
        return 0;
    } else if (m_simplex_type == PrimitiveType::Vertex) {
        return target.identity();
    } else {
        return subgroup::convert(m_simplex_type, target.m_simplex_type, permutation_index);
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
        wmtk::dart::permutation_index_from_tuple(m_simplex_type, t)};
}

int8_t SimplexDart::simplex_index(const Dart& dart, PrimitiveType simplex_type) const
{
    return simplex_index(dart.permutation(), simplex_type);
}
int8_t SimplexDart::simplex_index(const int8_t permutation_index, PrimitiveType simplex_type) const
{
    return utils::simplex_index_from_permutation_index(m_simplex_type, permutation_index, simplex_type);
}
} // namespace wmtk::dart
    */

}
}
