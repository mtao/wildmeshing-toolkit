#include "ExtremeOpt.h"
#include <Eigen/Core>
#include <igl/write_triangle_mesh.h>
#include <wmtk/utils/AMIPS2D.h>
#include <igl/predicates/predicates.h>
#include <tbb/concurrent_vector.h>

namespace extremeopt {

void ExtremeOpt::create_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::MatrixXd& uv)
{
    // Register attributes
    p_vertex_attrs = &vertex_attrs;

    // Convert from eigen to internal representation (TODO: move to utils and remove it from all app)
    std::vector<std::array<size_t, 3>> tri(F.rows());
    
    for (int i = 0; i < F.rows(); i++) 
        for (int j = 0; j < 3; j++) 
            tri[i][j] = (size_t)F(i, j);
    
    // Initialize the trimesh class which handles connectivity
    wmtk::TriMesh::create_mesh(V.rows(), tri);

    // Save the vertex position in the vertex attributes
    for (unsigned i = 0; i<V.rows();++i)
    {
        vertex_attrs[i].pos << uv.row(i)[0], uv.row(i)[1];
        vertex_attrs[i].pos << V.row(i)[0], V.row(i)[1], V.row(i)[2];
    }
}

void ExtremeOpt::export_mesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F)
{
    V = Eigen::MatrixXd::Zero(vert_capacity(), 2);
    for (auto& t : get_vertices()) {
        auto i = t.vid(*this);
        V.row(i) = vertex_attrs[i].pos;
    }

    F = Eigen::MatrixXi::Constant(tri_capacity(), 3, -1);
    for (auto& t : get_faces()) {
        auto i = t.fid(*this);
        auto vs = oriented_tri_vertices(t);
        for (int j = 0; j < 3; j++) {
            F(i, j) = vs[j].vid(*this);
        }
    }
}

void ExtremeOpt::write_obj(const std::string& path)
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    export_mesh(V,F);

    Eigen::MatrixXd V3 = Eigen::MatrixXd::Zero(V.rows(),3);
    V3.leftCols(2) = V;

    igl::writeOBJ(path,V3,F);
}


double ExtremeOpt::get_quality(const Tuple& loc) const
{
    // Global ids of the vertices of the triangle
    auto its = oriented_tri_vids(loc);

    // Temporary variable to store the stacked coordinates of the triangle
    std::array<double, 6> T;
    auto energy = -1.;
    for (auto k = 0; k < 3; k++)
        for (auto j = 0; j < 2; j++) 
            T[k * 2 + j] = vertex_attrs[its[k]].pos[j];

    // Energy evaluation
    energy = wmtk::AMIPS2D_energy(T);

    // Filter for numerical issues
    if (std::isinf(energy) || std::isnan(energy)) 
        return MAX_ENERGY;

    return energy;
}

Eigen::VectorXd ExtremeOpt::get_quality_all_triangles()
{
    // Use a concurrent vector as for_each_face is parallel
    tbb::concurrent_vector<double> quality;
    quality.reserve(vertex_attrs.size());

    // Evaluate quality in parallel
    for_each_face(
        [&](auto& f) {
            quality.push_back(get_quality(f));
        }
    );

    // Copy back in a VectorXd
    Eigen::VectorXd ret(quality.size());
    for (unsigned i=0; i<quality.size();++i)
        ret[i] = quality[i];
    return ret;
}

bool ExtremeOpt::is_inverted(const Tuple& loc) const
{
    // Get the vertices ids
    auto vs = oriented_tri_vertices(loc);

    igl::predicates::exactinit();

    // Use igl for checking orientation
    auto res = igl::predicates::orient2d(
        vertex_attrs[vs[0].vid(*this)].pos,
        vertex_attrs[vs[1].vid(*this)].pos,
        vertex_attrs[vs[2].vid(*this)].pos);

    // The element is inverted if it not positive (i.e. it is negative or it is degenerate)
    return (res != igl::predicates::Orientation::POSITIVE);
}

}