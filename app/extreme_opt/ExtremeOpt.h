#pragma once

#include <igl/PI.h>
#include <igl/Timer.h>
#include <wmtk/TriMesh.h>
#include <wmtk/TriMeshOperation.h>
#include <nlohmann/json.hpp>
#include "Parameters.h"
// #include <fastenvelope/FastEnvelope.h>
#include <igl/AABB.h>
#include <Eigen/Sparse>
#include <sec/envelope/SampleEnvelope.hpp>


// #define OPT_MAX

namespace extremeopt {

void get_grad_op(
    const Eigen::MatrixXd& V,
    const Eigen::MatrixXi& F,
    Eigen::SparseMatrix<double>& grad_op);

class VertexAttributes
{
public:
    Eigen::Vector2d pos = Eigen::Vector2d::Zero();
    Eigen::Vector3d pos_3d = Eigen::Vector3d::Zero();

    size_t partition_id = 0; // TODO this should not be here

    // Vertices marked as fixed cannot be modified by any local operation
    bool fixed = false;
};


class FaceAttributes
{
public:
    double area_3d;
};

class EdgeAttributes
{
public:
    // the other edge that this is constrained with in counterclockwise order
    // if this halfedge points this way
    //  --->
    //  -------
    //  the other halfedge ponts this way across the seam
    //  <---
    wmtk::TriMesh::Tuple pair;
};

class ExtremeOpt : public wmtk::TriMesh
{
public:
    sample_envelope::SampleEnvelope m_envelope;
    Parameters m_params;
    // Energy Assigned to undefined energy
    // TODO: why not the max double?
    const double MAX_ENERGY = 1e50;

    igl::AABB<Eigen::MatrixXd, 3> tree; // for closest point queries
    Eigen::MatrixXd input_V;
    Eigen::MatrixXi input_F;

    double elen_threshold;
    double elen_threshold_3d;

    bool has_degenerate_tris(const std::vector<Tuple>& tris) const;

    ExtremeOpt(){};

    virtual ~ExtremeOpt(){};

    // Store the per-vertex and per-face attributes
    wmtk::AttributeCollection<VertexAttributes> vertex_attrs;
    wmtk::AttributeCollection<FaceAttributes> face_attrs;
    wmtk::AttributeCollection<EdgeAttributes> edge_attrs;

    void consolidate_mesh_cons();

    bool check_constraints(double eps = 1e-7);

    struct PositionInfoCache
    {
        int vid1;
        int vid2;
        Eigen::Vector3d V1;
        Eigen::Vector3d V2;
        Eigen::Vector2d uv1;
        Eigen::Vector2d uv2;
        bool is_v1_bd;
        bool is_v2_bd;
        Tuple bd_e1;
        Tuple bd_e2;
        double E_before_collapse;
    };
    tbb::enumerable_thread_specific<PositionInfoCache> position_cache;

    struct SwapInfoCache
    {
        Tuple t1;
        Tuple t2;
        double E_old;
    };
    tbb::enumerable_thread_specific<SwapInfoCache> swap_cache;
    // Initializes the mesh
    void create_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F, const Eigen::MatrixXd& uv);

    // Initialize the mesh with constraints
    void init_constraints(const std::vector<std::vector<int>>& EE_e);
    void update_constraints_EE_v(const Eigen::MatrixXi& EE);

    // Exports V and F of the stored mesh
    void export_mesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F, Eigen::MatrixXd& uv) const;

    // Export constraints EE
    void export_EE(Eigen::MatrixXi& EE);

    // Writes a triangle mesh in OBJ format
    void write_obj(const std::string& path) const;

    // Computes the quality of a triangle
    double get_quality(const Tuple& loc) const;

    // Computes the average quality of a mesh
    Eigen::VectorXd get_quality_all_triangles();

    // compute the max_E of a one ring
    int get_mesh_onering(
        const Tuple& t,
        Eigen::MatrixXd& V_local,
        Eigen::MatrixXd& uv_local,
        Eigen::MatrixXi& F_local);
    double get_e_max_onering(const Tuple& t);
    double get_e_onering_edge(const Tuple& t);
    void get_mesh_onering_edge(
        const Tuple& t,
        Eigen::MatrixXd& V_local,
        Eigen::MatrixXd& uv_local,
        Eigen::MatrixXi& F_local);
    // Check if a triangle is inverted
    bool is_inverted(const Tuple& loc) const;

    // Optimization
    void do_optimization(nlohmann::json& opt_log);

    // Vertex Smoothing
    bool smooth_before(const Tuple& t);
    bool smooth_after(const Tuple& t);
    void smooth_all_vertices();
    void smooth_global(int steps);

    // Edge Swapping
    std::vector<wmtk::TriMesh::Tuple> new_edges_after(
        const std::vector<wmtk::TriMesh::Tuple>& tris) const;
    std::vector<wmtk::TriMesh::Tuple> replace_edges_after_split(
        const std::vector<wmtk::TriMesh::Tuple>& tris,
        const size_t vid_threshold) const;

    bool invariants(const std::vector<Tuple>& new_tris) override;

    bool swap_edge_before(const Tuple& t);
    bool swap_edge_after(const Tuple& t);
    void swap_all_edges();

    // Edge Collapsing
    void cache_edge_positions(const Tuple& t);
    bool collapse_edge_before(const Tuple& t);
    bool collapse_edge_after(const Tuple& t);
    void collapse_all_edges();
    bool collapse_bd_edge_after(
        const Tuple& t,
        const Eigen::Vector3d& V_keep,
        const Eigen::Vector2d& uv_keep,
        Tuple& t_l_old,
        Tuple& t_r_old,
        double& E);
    // Edge Splitting
    bool split_edge_before(const Tuple& t);
    bool split_edge_after(const Tuple& t);
    void split_all_edges(const Eigen::VectorXd& Es);
};

} // namespace extremeopt
