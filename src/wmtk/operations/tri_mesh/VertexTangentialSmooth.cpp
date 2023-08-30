#include "VertexTangentialSmooth.hpp"

#include <wmtk/SimplicialComplex.hpp>
#include <wmtk/utils/mesh_utils.hpp>
#include "VertexSmooth.hpp"

namespace wmtk::operations::tri_mesh {
VertexTangentialSmooth::VertexTangentialSmooth(
    Mesh& m,
    const Tuple& t,
    const OperationSettings<VertexTangentialSmooth>& settings)
    : Operation(m)
    , m_tuple{t}
    , m_pos_accessor{m.create_accessor<double>(settings.position)}
    , m_settings{settings}
{}

std::string VertexTangentialSmooth::name() const
{
    return "tri_mesh_vertex_tangential_smooth";
}

bool VertexTangentialSmooth::before() const
{
    if (!mesh().is_valid_slow(m_tuple)) {
        return false;
    }
    if (!m_settings.smooth_boundary && mesh().is_boundary_vertex(m_tuple)) {
        return false;
    }
    return true;
}

bool VertexTangentialSmooth::execute()
{
    const Eigen::Vector3d p = m_pos_accessor.vector_attribute(m_tuple);
    {
        OperationSettings<tri_mesh::VertexSmooth> op_settings;
        op_settings.position = m_pos_accessor.handle();
        op_settings.smooth_boundary = m_settings.smooth_boundary;
        tri_mesh::VertexSmooth smooth_op(mesh(), m_tuple, op_settings);
        if (!smooth_op()) {
            return false;
        }

        m_tuple = smooth_op.return_tuple();
        assert(mesh().is_valid_slow(m_tuple));
    }
    const Eigen::Vector3d g = m_pos_accessor.vector_attribute(m_tuple); // center of gravity

    if (m_settings.smooth_boundary && mesh().is_boundary_vertex(m_tuple)) {
        //
        Tuple t0 = m_tuple;
        while (!mesh().is_boundary(t0)) {
            t0 = mesh().switch_edge(mesh().switch_face(t0));
        }
        const Tuple v0 = mesh().switch_vertex(t0);

        Tuple t1 = mesh().switch_edge(m_tuple);
        while (!mesh().is_boundary(t1)) {
            t1 = mesh().switch_edge(mesh().switch_face(t1));
        }
        const Tuple v1 = mesh().switch_vertex(t1);

        const Eigen::Vector3d p0 = m_pos_accessor.vector_attribute(v0);
        const Eigen::Vector3d p1 = m_pos_accessor.vector_attribute(v1);

        const Eigen::Vector3d tang = (p1 - p0).normalized();
        if (tang.squaredNorm() < 1e-10) {
            return false;
        }

        m_pos_accessor.vector_attribute(m_tuple) =
            p + m_settings.damping_factor * tang * tang.transpose() * (g - p);

    } else {
        const Eigen::Vector3d n =
            mesh_utils::compute_vertex_normal(mesh(), m_pos_accessor, m_tuple);

        if (n.squaredNorm() < 1e-10) {
            return false;
        }

        // following Botsch&Kobbelt - Remeshing for Multiresolution Modeling
        m_pos_accessor.vector_attribute(m_tuple) =
            p +
            m_settings.damping_factor * (Eigen::Matrix3d::Identity() - n * n.transpose()) * (g - p);
    }

    return true;
}


} // namespace wmtk::operations::tri_mesh