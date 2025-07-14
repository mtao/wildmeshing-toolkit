#include "BVHEnvelope.hpp"
#include <SimpleBVH/BVH.hpp>

namespace wmtk::components::envelope {

BVHEnvelope::BVHEnvelope(const Eigen::MatrixXd& V, const Eigen::MatrixXi& S, double envelope_size)
{
    m_bvh = std::make_shared<SimpleBVH::BVH>();
    m_bvh->init(V, S, 1e-10);
}
BVHEnvelope::~BVHEnvelope() = default;

bool BVHEnvelope::is_inside(const Vector3d& p) const {}
bool BVHEnvelope::is_inside(const Vector3r& p) const
{
    return is_inside(p.cast<double>().eval());
}

auto BVHEnvelope::nearest_point(const Vec3dMax& p) const -> std::tuple<Vec3dMax, double, int>
{
    SimpleBVH::VectorMax3d nearest_point;
    double sq_dist;
    int nearest_facet = -1;

    update_nearest_point(p, nearest_point, sq_dist, nearest_facet);
    return std::make_tuple(nearest_point, sq_dist, nearest_facet);
}
auto BVHEnvelope::edge_nearest_point(const Vec3dMax& p0, const Vec3dMax& p1) const
    -> std::tuple<Vec3dMax, double, int>
{
    int d = p0.rows();
    assert(p0.rows() == p1.rows());
    auto [np, sqd, f] = nearest_point(p0);
    Vec3dMax nearest_point = np;
    double sq_dist = sqd;
    int nearest_facet = f;

    const int64_t N = (p0 - p1).norm() / d + 1;
    for (int64_t n = 1; n < N; n++) {
        auto tmp = (p0 * (double(n) / N) + p1 * (N - double(n)) / N).eval();
        update_nearest_point(tmp, np, sqd, f);
        if (sqd < sq_dist) {
            nearest_point = np;
            sq_dist = sqd;
            nearest_facet = f;
        }
    }
    update_nearest_point(p1, np, sqd, f);
    if (sqd < sq_dist) {
        nearest_point = np;
        sq_dist = sqd;
        nearest_facet = f;
    }
    return std::make_tuple(nearest_point, sq_dist, nearest_facet);
}
void BVHEnvelope::update_nearest_point(
    const Vec3dMax& p,
    Vec3dMax& nearest_point,
    double& sq_dist,
    int& nearest_facet) const
{
    if (nearest_facet >= 0) {
        sq_dist = (p - nearest_point).squaredNorm();
    }
    m_bvh->nearest_facet_with_hint(p, nearest_facet, nearest_point, sq_dist);
}

bool BVHEnvelope::is_inside(const Vector3d& p0, const Vector3d& p1) const
{
    auto [np, sqd, f] = nearest_point(p0);
    const int64_t N = (p0 - p1).norm() / d + 1;
    for (int64_t n = 1; n < N; n++) {
        auto tmp = (p0 * (double(n) / N) + p1 * (N - double(n)) / N).eval();
        update_nearest_point(v, np, sqd, f);
        if (sqd > real_envelope_2) {
            wmtk::logger().debug("fail envelope check 5");
            return false;
        }
    }
    update_nearest_point(p1, np, sqd, f);
    if (sqd > real_envelope_2) {
        wmtk::logger().debug("fail envelope check 5");
        return false;
    }
    return true;
}
bool BVHEnvelope::is_inside(const Mesh& m, const Tuple& tuple, const PrimitiveType pt) const
{
    SimpleBVH::VectorMax3d nearest_point;
    double sq_dist;

    const double d = m_envelope_size;
    const double real_envelope = m_envelope_size - d / sqrt(accessor.dimension());
    const double real_envelope_2 = real_envelope * real_envelope;

    std::vector<SimpleBVH::VectorMax3d> pts;
    if (pt == PrimitiveType::Edge) {
        const auto p0 = accessor.const_vector_attribute(tuple).cast<double>().eval();
        const auto p1 =
            accessor.const_vector_attribute(mesh().switch_tuple(tuple, PV)).cast<double>().eval();

        const int64_t N = (p0 - p1).norm() / d + 1;
        pts.reserve(N);

        for (int64_t n = 0; n <= N; n++) {
            auto tmp = (p0 * (double(n) / N) + p1 * (N - double(n)) / N).eval();
            pts.push_back(tmp);
        }

        auto current_point = pts[0];

        int prev_facet = m_bvh->nearest_facet(current_point, nearest_point, sq_dist);
        if (sq_dist > real_envelope_2) {
            wmtk::logger().debug("fail envelope check 4");
            return false;
        }

        for (const auto& v : pts) {
            sq_dist = (v - nearest_point).squaredNorm();
            m_bvh->nearest_facet_with_hint(v, prev_facet, nearest_point, sq_dist);
            if (sq_dist > real_envelope_2) {
                wmtk::logger().debug("fail envelope check 5");
                return false;
            }
        }

        return true;
    } else if (type == PrimitiveType::Vertex) {
        for (const Tuple& tuple : top_dimension_tuples_after) {
            Eigen::Vector3d p = accessor.const_vector_attribute(tuple).cast<double>();
            m_bvh->nearest_facet(p, nearest_point, sq_dist);
            if (sq_dist > m_envelope_size * m_envelope_size) {
                wmtk::logger().debug("fail envelope check 6");
                return false;
            }
        }

        return true;
    } else {
        throw std::runtime_error("Invalid mesh type");
    }
    return true;
}
}
else if (m_coordinate_handle.holds<double>())
{
    const attribute::Accessor<double> accessor =
        mesh().create_const_accessor(m_coordinate_handle.as<double>());
    const auto type = mesh().top_simplex_type();

    assert(m_bvh);

    SimpleBVH::VectorMax3d nearest_point;
    double sq_dist;

    const double d = m_envelope_size;
    const double real_envelope = m_envelope_size - d / sqrt(accessor.dimension());
    const double real_envelope_2 = real_envelope * real_envelope;

    if (type == PrimitiveType::Edge) {
        std::vector<SimpleBVH::VectorMax3d> pts;

        int prev_facet = m_bvh->nearest_facet(current_point, nearest_point, sq_dist);
        for (const Tuple& tuple : top_dimension_tuples_after) {
            SimpleBVH::VectorMax3d p0 = accessor.const_vector_attribute(tuple).cast<double>();
            SimpleBVH::VectorMax3d p1 =
                accessor.const_vector_attribute(mesh().switch_tuple(tuple, PV)).cast<double>();

            const int64_t N = (p0 - p1).norm() / d + 1;
            // pts.reserve(pts.size() + N);

            for (int64_t n = 0; n <= N; n++) {
                auto tmp = (p0 * (double(n) / N) + p1 * (N - double(n)) / N).eval();
                // pts.push_back(tmp);
            }
        }

        auto current_point = pts[0];

        if (sq_dist > real_envelope_2) {
            wmtk::logger().debug("fail envelope check 10");
            return false;
        }

        for (const auto& v : pts) {
            sq_dist = (v - nearest_point).squaredNorm();
            m_bvh->nearest_facet_with_hint(v, prev_facet, nearest_point, sq_dist);
            if (sq_dist > real_envelope_2) {
                wmtk::logger().debug("fail envelope check 11");
                return false;
            }
        }

        return true;
    } else if (type == PrimitiveType::Vertex) {
        for (const Tuple& tuple : top_dimension_tuples_after) {
            Eigen::Vector3d p = accessor.const_vector_attribute(tuple).cast<double>();
            m_bvh->nearest_facet(p, nearest_point, sq_dist);
            if (sq_dist > m_envelope_size * m_envelope_size) {
                wmtk::logger().debug("fail envelope check 12");
                return false;
            }
        }

        return true;
    } else {
        throw std::runtime_error("Invalid mesh type");
    }
    return true;
}
}
else
{
    throw std::runtime_error("Envelope mesh handle type invlid");
}
}
}
