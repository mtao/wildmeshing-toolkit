#pragma once
#include <memory>
#include "Envelope.hpp"


namespace wmtk::components::envelope {
namespace SimpleBVH {
class BVH;
class VectorMax3d;
} // namespace SimpleBVH


class BVHEnvelope : public Envelope
{
public:
    // Vec3d matches simpleBVH's vec3d
    using Vec3dMax = Eigen::Matrix<double, Eigen::Dynamic, 1, Eigen::ColMajor, 3, 1>;
    // works for edge and vertex only
    BVHEnvelope(const Eigen::MatrixXd& V, const Eigen::MatrixXi& S, double envelope_size);
    ~BVHEnvelope() final override;
    bool is_inside(const Vector3d& p) const final override;
    bool is_inside(const Vector3r& p) const final override;
    bool is_inside(const Vector3d& start, const Vector3d& b) const;

    // position, distance_squard, nearest facet
    std::tuple<Vec3dMax, double, int> nearest_point(const Vec3dMax& start) const;
    void update_nearest_point(
        const Vec3dMax& start,
        Vec3dMax& nearest_point,
        double& sq_dist,
        int& nearest_facet) const;

    std::tuple<Vec3d, double, int> edge_nearest_point(const Vec3dMax& start, const Vec3dMax& end)
        const;

private:
    std::shared_ptr<SimpleBVH::BVH> m_bvh = nullptr;
    double m_envelope_size = 1e-3;
};
} // namespace wmtk::components::envelope
