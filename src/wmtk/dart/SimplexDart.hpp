#pragma once
#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>

#include "Dart.hpp"
namespace wmtk::dart {
// TODO: permutation_index here currently stands for the range [0,N] rfather than the set of
// discontiguous valid indices stored in each mesh. This nomenclature needs to be cleaned up.
class SimplexDart
{
public:
    // to avoid potential construction costs we have some singletons available
    static const SimplexDart& get_singleton(wmtk::PrimitiveType simplex_type);
    virtual ~SimplexDart();
    virtual int8_t product(int8_t a, int8_t b) const = 0;
    virtual int8_t inverse(int8_t a) const = 0;
    virtual int8_t identity() const = 0;
    virtual int8_t opposite() const = 0;
    virtual size_t size() const = 0;
    virtual VectorX<int8_t>::ConstMapType permutation_indices() const = 0;
    virtual wmtk::PrimitiveType simplex_type() const = 0;
    virtual int8_t simplex_index(int8_t permutation_index, wmtk::PrimitiveType type) const = 0;
    // returns the action equivalent to switching by a particular primitive
    virtual int8_t permutation_index_from_primitive_switch(wmtk::PrimitiveType pt) const = 0;


    Dart act(const Dart& d, int8_t action) const;
    int8_t act(int8_t permutation, int8_t action) const;
    wmtk::Tuple as_tuple(int64_t gid, int8_t permutation_index) const;
    wmtk::Tuple update_tuple_from_permutation(const Tuple& t, int8_t permutation_index) const;

    wmtk::Tuple as_tuple(const Dart& dart) const;
    Dart as_dart(const wmtk::Tuple& t) const;

    int8_t get_permutation(const wmtk::Tuple& t) const;


    int8_t simplex_index(const Dart& dart, PrimitiveType simplex_type) const;

    wmtk::Tuple tuple_from_permutation_index(int64_t gid, int8_t permutation_index) const;
    wmtk::Tuple update_tuple_from_permutation_index(const Tuple& t, int8_t permutation_index) const;

    wmtk::Tuple tuple_from_dart(const Dart& dart) const;
    Dart dart_from_tuple(const wmtk::Tuple& t) const;

    int8_t permutation_index_from_tuple(const wmtk::Tuple& t) const;

    int8_t primitive_as_index(wmtk::PrimitiveType pt) const
    {
        return permutation_index_from_primitive_switch(pt);
    }


    // converts input valid_indx to the target mesh
    int8_t convert(int8_t permutation_index, const SimplexDart& target) const;
};
/*
class SimplexDart
{
public:
    SimplexDart(wmtk::PrimitiveType simplex_type);

    // to avoid potential construction costs we have some singletons available
    static const SimplexDart& get_singleton(wmtk::PrimitiveType simplex_type);

    // takes two valid indices and returns their dart-product as a valid index
    int8_t product(int8_t a, int8_t b) const;
    int8_t inverse(int8_t a) const;

    // returns the action equivalent to switching by a particular primitive
    int8_t primitive_as_index(wmtk::PrimitiveType pt) const;

    // returns the two-sided identity action - product(identity(),x) = x
    int8_t identity() const;
    // returns the dart O such that for each simplex I[i] in the identity I,
    // O[i] = S - I[i], where S is the facet simplex
    int8_t opposite() const;

    Dart act(const Dart& d, int8_t action) const;

    wmtk::Tuple tuple_from_permutation_index(int64_t gid, int8_t permutation_index) const;
    wmtk::Tuple update_tuple_from_permutation_index(const Tuple& t, int8_t permutation_index) const;

    wmtk::Tuple tuple_from_dart(const Dart& dart) const;
    Dart dart_from_tuple(const wmtk::Tuple& t) const;

    int8_t permutation_index_from_tuple(const wmtk::Tuple& t) const;


    int8_t simplex_index(const int8_t permutation_index, PrimitiveType simplex_type) const;
    int8_t simplex_index(const Dart& dart, PrimitiveType simplex_type) const;

    using binary_op_type = int8_t (*)(int8_t, int8_t);
    using unary_op_type = int8_t (*)(int8_t);
    using primitive_to_index_type = int8_t (*)(PrimitiveType);
    using nullary_op_type = int8_t (*)();

    size_t size() const;
    VectorX<int8_t>::ConstMapType valid_indices() const;


    // converts input valid_indx to the target mesh
    int8_t convert(int8_t permutation_index, const SimplexDart& target) const;


    wmtk::PrimitiveType simplex_type() const { return m_simplex_type; }


private:
    const wmtk::PrimitiveType m_simplex_type;
    const binary_op_type m_product;
    const unary_op_type m_inverse;
    const primitive_to_index_type m_primitive_to_index;
    const nullary_op_type m_identity;
    const nullary_op_type m_opposite;
};
*/
} // namespace wmtk::dart
