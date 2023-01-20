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
    void before_hook(const TriMesh& m, const std::set<TriMesh::Tuple>& input_tris);
    void after_hook(const TriMesh& m, const std::set<TriMesh::Tuple>& output_tris);

    // derived class is required to store a global representation of the point, used in before_hook
    virtual void update_global_coordinate(const TriMesh& m, size_t point_index) = 0;


protected:

    // derived class is required to store a global representation of the point, used in before_hook
    void update_local_coordinate(
        const TriMesh& m,
        size_t point_index,
        const std::set<size_t>& possible_tris);
    // derived class is required to identify which point and triangle
    virtual bool point_in_triangle(const TriMesh& m, const std::array<size_t,3>& triangle_indices , size_t point_index)
        const = 0;

    virtual std::array<double, 3>
    get_barycentric(const TriMesh& m, const std::array<size_t,3>& triangle_indices, size_t point_index) const = 0;


protected:
    // local representation of points in a triangle mesh
    tbb::concurrent_vector<TriMesh::Tuple> triangle_tuples;
    tbb::concurrent_vector<std::array<double, 3>> barycentric_coordinates;

    tbb::enumerable_thread_specific<std::set<size_t>> active_points;

    tbb::concurrent_vector<std::set<size_t>> active_point_bins;
};

template <typename PointType>
class TransportablePoints: public TransportablePointsBase
{
public:
    // derived class is required to store a global representation of the point, used in
    // before_hook
    void update_global_coordinate(const TriMesh& m, size_t point_index) override;

    // predicate to determine whether a point lies in a particular triangle
    bool point_in_triangle(const TriMesh& m, size_t triangle_index, size_t point_index)
        const override;

    // computes the barycentric coordinates for the point at point_index assuming that it lies in
    // triangle_index
    std::array<double, 3>
    get_barycentric(const TriMesh& m, size_t triangle_index, size_t point_index) const override;


#if defined(USE_CALLBACK_FOR_TRANSPOORTABLE_POINTS)

    using BarycentricInterpFuncType = std::function<PointType(
        const TriMesh&,
        const std::array<double, 3>&,
        const std::array<std::reference_wrapper<const PointType>, 3>&)>;
    using PointInTriangleFuncType = std::function<bool(const TriMesh&, size_t, size_t)>;
    using GetBarycentricFuncType =
        std::function<std::array<double, 3>(const TriMesh&, size_t, size_t)>;

    BarycentricInterpFuncType barycentric_interp_callback;
    PointInTriangleFuncType point_in_triangle_callback;
    GetBarycentricFuncType get_barycentric_callback;
#endif
    // global coordinates
    tbb::concurrent_vector<PointType> points_global;
};

#if defined(USE_CALLBACK_FOR_TRANSPOORTABLE_POINTS)
template <typename PointType>
void TransportablePoints<PointType>::update_global_coordinate(const TriMesh& m, size_t point_index)
{
    const std::array<size_t, 3>& vertex_indices = m.oriented_tri_vids(triangle_tuples[point_index]);
    const tbb::concurrent_vector<PointType>& P =
        dynamic_cast<const AttributeCollection<PointType>&>(*m.p_vertex_attrs).m_attributes;
    const std::array<std::reference_wrapper<const PointType>, 3> points{
        {P[vertex_indices[0]], P[vertex_indices[1]], P[vertex_indices[2]]}};

    points_global[point_index] = barycentric_interp_callback(
        m,
        TransportablePointsBase::triangle_tuples[point_index],
        TransportablePointsBase::barycentric_coordinates[point_index],
        points);
}

template <typename PointType>
bool TransportablePoints<PointType>::point_in_triangle(
    const TriMesh& m,
    size_t triangle_index,
    size_t point_index) const
{
    return point_in_triangle_callback(m, triangle_index, point_index);
}

template <typename PointType>
std::array<double, 3> TransportablePoints<PointType>::get_barycentric(
    const TriMesh& m,
    size_t triangle_index,
    size_t point_index) const
{
    return get_barycentric_callback(m, triangle_index, point_index);
}

#endif


} // namespace wmtk
