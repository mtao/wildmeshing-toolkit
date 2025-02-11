#include <wmtk/Types.hpp>
#include <wmtk/attribute/Accessor.hpp>
#include <wmtk/multimesh/MultiMeshManager.hpp>
#if defined(WMTK_ENABLED_MULTIMESH_DART)
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_simplex_involution.hpp>
#endif

// #define WMTK_DISABLE_COMPRESSED_MULTIMESH_TUPLE
namespace wmtk::multimesh::utils {


template <typename MeshA, typename MeshB>
void symmetric_write_tuple_map_attributes(
    wmtk::attribute::Accessor<int64_t, MeshA>& a_to_b,
    wmtk::attribute::Accessor<int64_t, MeshB>& b_to_a,
    const Tuple& a_tuple,
    const Tuple& b_tuple)
{
    write_tuple_map_attribute(a_to_b, a_tuple, b_tuple);
    write_tuple_map_attribute(b_to_a, b_tuple, a_tuple);
}

#if defined(WMTK_ENABLED_MULTIMESH_DART)
template <typename MeshA, typename MeshB>
void symmetric_write_tuple_map_attributes(
    wmtk::dart::DartAccessor<1, MeshA>& a_to_b,
    wmtk::dart::DartAccessor<1, MeshB>& b_to_a,
    const Tuple& a_tuple,
    const Tuple& b_tuple)
{
    const PrimitiveType apt = a_to_b.mesh().top_simplex_type();
    const PrimitiveType bpt = b_to_a.mesh().top_simplex_type();
    const dart::SimplexDart& sd = dart::SimplexDart::get_singleton(apt);
    const dart::SimplexDart& osd = dart::SimplexDart::get_singleton(bpt);
    auto a = sd.dart_from_tuple(a_tuple);
    auto b = osd.dart_from_tuple(b_tuple);
    std::tie(a_to_b[a], b_to_a[b]) = wmtk::dart::utils::get_simplex_involution_pair(apt, a, bpt, b);
}
#else

#if defined WMTK_DISABLE_COMPRESSED_MULTIMESH_TUPLE
constexpr static int64_t TUPLE_SIZE = 4; // in terms of int64_t
constexpr static int64_t GLOBAL_ID_INDEX = 3;
#else
constexpr static int64_t TUPLE_SIZE = 2; // in terms of int64_t
constexpr static int64_t GLOBAL_ID_INDEX = 0;
#endif
constexpr static int64_t DEFAULT_TUPLES_VALUES = -1;
constexpr static int64_t TWO_TUPLE_SIZE = TUPLE_SIZE * 2; // in terms of int64_t
using TupleVector = Vector<int64_t, TUPLE_SIZE>;
using TwoTupleVector = Vector<int64_t, TWO_TUPLE_SIZE>;


Vector<int64_t, TUPLE_SIZE> tuple_to_vector(const Tuple& t);
Tuple vector_to_tuple(const Eigen::Ref<const TupleVector>& v);

std::tuple<Tuple, Tuple> vectors_to_tuples(const Eigen::Ref<const TwoTupleVector>& v);
TwoTupleVector tuples_to_vectors(const Tuple& a, const Tuple& b);

// utility functions to simplify how we encode 2-Tuple attributes as 10-int64_t attribute
template <typename MeshType>
void write_tuple_map_attribute(
    wmtk::attribute::Accessor<int64_t, MeshType>& map_accessor,
    const Tuple& source_tuple,
    const Tuple& target_tuple);

template <typename MeshType>
std::tuple<Tuple, Tuple> read_tuple_map_attribute(
    const wmtk::attribute::Accessor<int64_t, MeshType>& accessor,
    const Tuple& source_tuple);
void write_tuple_map_attribute_slow(
    Mesh& source_mesh,
    TypedAttributeHandle<int64_t> map_handle,
    const Tuple& source_tuple,
    const Tuple& target_tuple);


std::tuple<Tuple, Tuple> read_tuple_map_attribute_slow(
    const Mesh& source_mesh,
    TypedAttributeHandle<int64_t> map_handle,
    const Tuple& source_tuple);
#endif
} // namespace wmtk::multimesh::utils
