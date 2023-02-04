#pragma once

#define USE_CALLBACK_FOR_TRANSPOORTABLE_POINTS

#include <wmtk/TriMesh.h>
#include <functional>


namespace wmtk {
class TransportablePointsBase
{
public:
    virtual ~TransportablePointsBase();
    // convenience function that just calls
    void before_hook(const TriMesh& m, const std::vector<TriMesh::Tuple>& input_tris);
    void after_hook(const TriMesh& m, const std::vector<TriMesh::Tuple>& output_tris);

    // derived class is required to store a global representation of the point, used in before_hook
    virtual void update_global_coordinate(const TriMesh& m, size_t point_index) = 0;


protected:
    // derived class is required to store a global representation of the point, used in before_hook
    void update_local_coordinate(
        const TriMesh& m,
        size_t point_index,
        const std::vector<TriMesh::Tuple>& possible_tris);
    // derived class is required to identify which point and triangle
    virtual bool point_in_triangle(const TriMesh& m, const TriMesh::Tuple& t, size_t point_index)
        const = 0;

    virtual std::array<double, 3>
    get_barycentric(const TriMesh& m, const TriMesh::Tuple& t, size_t point_index) const = 0;


protected:
    // local representation of points in a triangle mesh
    tbb::concurrent_vector<TriMesh::Tuple> triangle_tuples;
    tbb::concurrent_vector<std::array<double, 3>> barycentric_coordinates;

    tbb::enumerable_thread_specific<std::set<size_t>> active_points;

    tbb::enumerable_thread_specific<std::set<size_t>> active_point_bins;
};

template <typename PointType>
class TransportablePoints : public TransportablePointsBase
{
public:
    using ThreePointType = const std::array<std::reference_wrapper<const PointType>, 3>;
    // derived class is required to store a global representation of the point, used in
    // before_hook
    void update_global_coordinate(const TriMesh& m, size_t point_index) override;

    // predicate to determine whether a point lies in a particular triangle
    bool point_in_triangle(const TriMesh& m, const TriMesh::Tuple& t, size_t point_index) const override;

    // computes the barycentric coordinates for the point at point_index assuming that it lies in
    // triangle_index
    std::array<double, 3>
    get_barycentric(const TriMesh& m, const TriMesh::Tuple& t, size_t point_index) const override;

    template <typename ContainerType>
    void set_points(const ContainerType& a) {
        points_global.resize(a.size());
        std::copy(std::begin(a),std::begin(a), std::begin(points_global));
    }

protected:
    const AttributeCollection<PointType>& get_vertex_attributes(const TriMesh& m) const;
    ThreePointType get_points(const TriMesh& m, const TriMesh::Tuple& t) const;

#if defined(USE_CALLBACK_FOR_TRANSPOORTABLE_POINTS)

    // Mesh being used, barycentric coordinate in the desired triangle, and three point values in
    // attribute array
    using BarycentricInterpFuncType = std::function<
        PointType(const TriMesh&, const TriMesh::Tuple&, const std::array<double, 3>&)>;
    // mesh being used, barycentric coordinate in each desired triangle
    using PointInTriangleFuncType =
        std::function<bool(const TriMesh&, const TriMesh::Tuple&, size_t)>;
    //
    using GetBarycentricFuncType =
        std::function<std::array<double, 3>(const TriMesh&, const TriMesh::Tuple&, size_t)>;

public:
    BarycentricInterpFuncType barycentric_interp_callback;
    PointInTriangleFuncType point_in_triangle_callback;
    GetBarycentricFuncType get_barycentric_callback;
#endif
    // global coordinates
    tbb::concurrent_vector<PointType> points_global;
};


template <typename PointType>
auto TransportablePoints<PointType>::get_vertex_attributes(const TriMesh& m) const
    -> const AttributeCollection<PointType>&
{
    return dynamic_cast<const AttributeCollection<PointType>&>(*m.p_vertex_attrs);
}

template <typename PointType>
auto TransportablePoints<PointType>::get_points(const TriMesh& m, const TriMesh::Tuple& t) const
    -> ThreePointType
{
    const tbb::concurrent_vector<PointType>& P = get_vertex_attributes(m).m_attributes;
    const auto [ai,bi,ci] = m.oriented_tri_vids(t);
    const PointType& a = P[ai];
    const PointType& b = P[bi];
    const PointType& c = P[ci];
    return {a, b, c};
}

#if defined(USE_CALLBACK_FOR_TRANSPOORTABLE_POINTS)
template <typename PointType>
void TransportablePoints<PointType>::update_global_coordinate(const TriMesh& m, size_t point_index)
{
    points_global[point_index] = barycentric_interp_callback(
        m,
        TransportablePointsBase::triangle_tuples[point_index],
        TransportablePointsBase::barycentric_coordinates[point_index]);
}

template <typename PointType>
bool TransportablePoints<PointType>::point_in_triangle(
    const TriMesh& m,
    const TriMesh::Tuple& t,
    size_t point_index) const
{
    return point_in_triangle_callback(m, t, point_index);
}

template <typename PointType>
std::array<double, 3> TransportablePoints<PointType>::get_barycentric(
    const TriMesh& m,
    const TriMesh::Tuple& t,
    size_t point_index) const
{
    return get_barycentric_callback(m, t, point_index);
}

#endif


} // namespace wmtk
