#include "TriWild.h"
#include <fastenvelope/FastEnvelope.h>
#include <igl/Timer.h>
#include <igl/predicates/predicates.h>
#include <igl/writeDMAT.h>
#include <igl/write_triangle_mesh.h>
#include <tbb/concurrent_vector.h>
#include <wmtk/utils/AMIPS2D.h>
#include <wmtk/utils/AMIPS2D_autodiff.h>
#include <Eigen/Core>
#include <wmtk/utils/TriQualityUtils.hpp>
#include <wmtk/utils/TupleUtils.hpp>
using namespace wmtk;

namespace triwild {
auto avg_edge_len = [](auto& m) {
    double avg_len = 0.0;
    auto edges = m.get_edges();
    for (auto& e : edges) avg_len += std::sqrt(m.get_length3d(e));
    return avg_len / edges.size();
};

void TriWild::set_parameters(
    const double target_edge_length,
    const std::function<Eigen::Vector3d(const double&, const double&)>& displacement_function,
    const ENERGY_TYPE energy_type,
    const bool boundary_parameter)
{
    mesh_parameters.m_target_l = target_edge_length;
    mesh_parameters.m_triwild_displacement = displacement_function;
    set_energy(
        energy_type); // set the displacement_function first since it is used for energy setting
    mesh_parameters.m_boundary_parameter = boundary_parameter;
}

/// @brief set the v as feature vertex, that is, it is a boundary vertex and the  incident boundary vertices are not colinear
/// @param v
void TriWild::set_feature(Tuple& v)
{
    // only set the feature if it is a boundary vertex
    assert(is_boundary_vertex(v));
    // get 3 vertices

    std::vector<size_t> incident_boundary_verts;
    auto one_ring = get_one_ring_edges_for_vertex(v);
    for (auto e : one_ring) {
        if (is_boundary_edge(e)) incident_boundary_verts.emplace_back(e.vid(*this));
    }
    // every boundary vertex has to have 2 neighbors that are boundary vertices
    assert(incident_boundary_verts.size() == 2);
    auto p1 = vertex_attrs[v.vid(*this)].pos;
    auto p2 = vertex_attrs[incident_boundary_verts[0]].pos;
    auto p3 = vertex_attrs[incident_boundary_verts[1]].pos;
    // check if they are co linear. set fixed = true if not
    double costheta = ((p2 - p1).stableNormalized()).dot((p3 - p1).stableNormalized()); // cos theta
    double theta = std::acos(std::clamp<double>(costheta, -1, 1));
    if (theta <= M_PI / 2)
        vertex_attrs[v.vid(*this)].fixed = true;
    else
        vertex_attrs[v.vid(*this)].fixed = false;
}

// assuming the m_triwild_displacement in mesh_parameter has been set
void TriWild::set_energy(ENERGY_TYPE energy_type)
{
    std::unique_ptr<Energy> energy_ptr;
    switch (energy_type) {
    case AMIPS: energy_ptr = std::make_unique<wmtk::AMIPS>(); break;
    case SYMDI: energy_ptr = std::make_unique<wmtk::SymDi>(); break;
    case EDGE_LENGTH:
        energy_ptr =
            std::make_unique<wmtk::EdgeLengthEnergy>(mesh_parameters.m_triwild_displacement);
        break;
    }
    mesh_parameters.m_energy = std::move(energy_ptr);
}

void TriWild::set_projection()
{
    struct Data
    {
        RowMatrix2<Index> E;
        RowMatrix2<Scalar> V;
        lagrange::bvh::EdgeAABBTree<RowMatrix2<Scalar>, RowMatrix2<Index>, 2> aabb;
    };
    auto data = std::make_shared<Data>();
    data->E = get_bnd_edge_matrix();
    RowMatrix2<Scalar> V_aabb = Eigen::MatrixXd::Zero(vert_capacity(), 2);
    for (int i = 0; i < vert_capacity(); ++i) {
        V_aabb.row(i) << vertex_attrs[i].pos[0], vertex_attrs[i].pos[1];
    }
    data->V = V_aabb;
    data->aabb =
        lagrange::bvh::EdgeAABBTree<RowMatrix2<Scalar>, RowMatrix2<Index>, 2>(data->V, data->E);
    std::function<Eigen::RowVector2d(const Eigen::RowVector2d& p)> projection =
        [data = std::move(data)](const Eigen::RowVector2d& p) -> Eigen::RowVector2d {
        uint64_t ind = 0;
        double distance = 0.0;
        static Eigen::RowVector2d p_ret;
        assert(data != nullptr);
        (data->aabb).get_closest_point(p, ind, p_ret, distance);
        return p_ret;
    };

    mesh_parameters.m_get_closest_point = std::move(projection);
}

bool TriWild::invariants(const std::vector<Tuple>& new_tris)
{
    if (mesh_parameters.m_has_envelope) {
        for (auto& t : new_tris) {
            std::array<Eigen::Vector3d, 3> tris;
            auto vs = oriented_tri_vertices(t);
            for (auto j = 0; j < 3; j++) {
                tris[j] << vertex_attrs[vs[j].vid(*this)].pos(0),
                    vertex_attrs[vs[j].vid(*this)].pos(1), 0.0;
            }
            if (mesh_parameters.m_envelope.is_outside(tris)) {
                wmtk::logger().info("envelop invariant fail");
                return false;
            }
        }
    }

    for (auto& t : new_tris) {
        if (is_inverted(t)) {
            wmtk::logger().info("inverted triangle {} ", t.fid(*this));
            return false;
        }
        assert(!is_inverted(t));
        Eigen::Vector2d a, b, c;
        auto verts = oriented_tri_vertices(t);
        assert(verts.size() == 3);
        a << vertex_attrs[verts[0].vid(*this)].pos(0), vertex_attrs[verts[0].vid(*this)].pos(1);
        b << vertex_attrs[verts[1].vid(*this)].pos(0), vertex_attrs[verts[1].vid(*this)].pos(1);
        c << vertex_attrs[verts[2].vid(*this)].pos(0), vertex_attrs[verts[2].vid(*this)].pos(1);

        // check both inverted and exact colinear
        if (wmtk::orient2d_t(a, b, c) != 1) {
            wmtk::logger().info("false in orientation and collinear {}", wmtk::orient2d_t(a, b, c));
            return false;
        }

        // add area check (degenerate tirangle)
        Eigen::Vector3d A, B, C;
        A.topRows(2) = a;
        A(2) = 0.;
        B.topRows(2) = b;
        B(2) = 0.;
        C.topRows(2) = c;
        C(2) = 0.;

        double area = ((B - A).cross(C - A)).squaredNorm();
        if (area < 1e-6 * ((B - A).squaredNorm() + (C - A).squaredNorm() + (B - C).squaredNorm())) {
            wmtk::logger().info("false in area ");
            wmtk::logger().info("failing ABC are {} {} {}", A, B, C);
            return false; // arbitrary chosen previous std::numeric_limits<double>::denorm_min() is
                          // too small}
        }
    }
    return true;
}
std::vector<TriMesh::Tuple> TriWild::new_edges_after(const std::vector<TriMesh::Tuple>& tris) const
{
    std::vector<TriMesh::Tuple> new_edges;

    for (auto t : tris) {
        for (auto j = 0; j < 3; j++) {
            new_edges.push_back(tuple_from_edge(t.fid(*this), j));
        }
    }
    wmtk::unique_edge_tuples(*this, new_edges);
    return new_edges;
}

void TriWild::create_mesh(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F)
{
    std::vector<Eigen::Vector3d> V_env;
    V_env.resize(V.rows());
    std::vector<Eigen::Vector3i> F_env;
    F_env.resize(F.rows());
    // Register attributes
    p_vertex_attrs = &vertex_attrs;
    // Convert from eigen to internal representation (TODO: move to utils and remove it from all
    // app)
    std::vector<std::array<size_t, 3>> tri(F.rows());

    for (int i = 0; i < F.rows(); i++) {
        F_env[i] << (size_t)F(i, 0), (size_t)F(i, 1), (size_t)F(i, 2);
        for (int j = 0; j < 3; j++) {
            tri[i][j] = (size_t)F(i, j);
        }
    }
    // Initialize the trimesh class which handles connectivity
    wmtk::TriMesh::create_mesh(V.rows(), tri);
    // Save the vertex position in the vertex attributes
    for (unsigned i = 0; i < V.rows(); ++i) {
        vertex_attrs[i].pos << V.row(i)[0], V.row(i)[1];
        V_env[i] << V.row(i)[0], V.row(i)[1], 0.0;
    }
    for (auto tri : this->get_faces()) {
        assert(!is_inverted(tri));
    }
    // construct the boundary map for boundary parametrization
    if (mesh_parameters.m_boundary_parameter) mesh_parameters.m_boundary.construct_boudaries(V, F);

    // mark boundary vertices as boundary_vertex
    // but this is not indiscriminatively rejected for all operations
    // other operations are conditioned on whether m_bnd_freeze is turned on
    // also obtain the boudnary parametrizatin too
    for (auto v : this->get_vertices()) {
        if (is_boundary_vertex(v)) {
            vertex_attrs[v.vid(*this)].boundary_vertex = is_boundary_vertex(v);
            set_feature(v);
            vertex_attrs[v.vid(*this)].curve_id = 0; /// only one connected mesh for now
            vertex_attrs[v.vid(*this)].t =
                mesh_parameters.m_boundary.uv_to_t(vertex_attrs[v.vid(*this)].pos);
        }
    }

    for (auto v : get_vertices()) {
        assert(vertex_attrs[v.vid(*this)].t >= 0);
    }

    for (auto v : get_vertices()) {
        if (is_boundary_vertex(v))
            assert(
                (vertex_attrs[v.vid(*this)].pos - mesh_parameters.m_boundary.t_to_uv(
                                                      vertex_attrs[v.vid(*this)].curve_id,
                                                      vertex_attrs[v.vid(*this)].t))
                    .squaredNorm() < 1e-8);
    }
}

Eigen::Matrix<uint64_t, Eigen::Dynamic, 2, Eigen::RowMajor> TriWild::get_bnd_edge_matrix()
{
    int num_bnd_edge = 0;
    for (auto e : get_edges()) {
        if (is_boundary_edge(e)) num_bnd_edge++;
    }
    Eigen::Matrix<uint64_t, Eigen::Dynamic, 2, Eigen::RowMajor> E(num_bnd_edge, 2);
    int i = 0;
    for (auto e : get_edges()) {
        if (is_boundary_edge(e)) {
            E.row(i) << (uint64_t)e.vid(*this), (uint64_t)e.switch_vertex(*this).vid(*this);
            i++;
        }
    }
    return E;
}

void TriWild::export_mesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F)
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

void TriWild::write_obj(const std::string& path)
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    export_mesh(V, F);

    Eigen::MatrixXd V3 = Eigen::MatrixXd::Zero(V.rows(), 3);
    V3.leftCols(2) = V;

    igl::writeOBJ(path, V3, F);
}

void TriWild::write_displaced_obj(
    const std::string& path,
    const std::function<double(double, double)>& displacement)
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    export_mesh(V, F);

    Eigen::MatrixXd V3 = Eigen::MatrixXd::Zero(V.rows(), 3);
    for (int i = 0; i < V.rows(); i++) {
        V3.row(i) << V(i, 0), V(i, 1), displacement(V(i, 0), V(i, 1));
    }

    igl::writeOBJ(path, V3, F);
}

void TriWild::write_displaced_obj(
    const std::string& path,
    const std::function<Eigen::Vector3d(double, double)>& displacement)
{
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;

    export_mesh(V, F);

    Eigen::MatrixXd V3 = Eigen::MatrixXd::Zero(V.rows(), 3);
    for (int i = 0; i < V.rows(); i++) {
        V3.row(i) = displacement(V(i, 0), V(i, 1));
    }

    igl::writeOBJ(path, V3, F);
}

double TriWild::get_length2d(const Tuple& t) const
{
    auto& m = *this;
    auto& v1 = t;
    auto v2 = t.switch_vertex(m);
    double length = (m.vertex_attrs[v1.vid(m)].pos - m.vertex_attrs[v2.vid(m)].pos).squaredNorm();
    return length;
}
double TriWild::get_length3d(const Tuple& t) const
{
    auto& v1 = t;
    auto v2 = t.switch_vertex(*this);
    auto v12d = vertex_attrs[v1.vid(*this)].pos;
    auto v22d = vertex_attrs[v2.vid(*this)].pos;

    double length = 0.0;

    // add 3d displacement
    auto v13d = mesh_parameters.m_triwild_displacement(v12d(0), v12d(1));
    auto v23d = mesh_parameters.m_triwild_displacement(v22d(0), v22d(1));
    length = (v13d - v23d).squaredNorm();
    return length;
}

double TriWild::get_quality(const Tuple& loc, int idx) const
{
    // Global ids of the vertices of the triangle
    auto its = oriented_tri_vids(loc);

    // Temporary variable to store the stacked coordinates of the triangle
    std::array<double, 6> T;
    auto energy = -1.;
    for (auto k = 0; k < 3; k++)
        for (auto j = 0; j < 2; j++) T[k * 2 + j] = vertex_attrs[its[k]].pos[j];

    // Energy evaluation
    // energy = wmtk::AMIPS2D_energy(T);
    wmtk::State state = {};
    state.input_triangle = T;
    state.scaling = mesh_parameters.m_target_l;
    state.idx = idx;
    mesh_parameters.m_energy->eval(state);
    energy = state.value;

    // Filter for numerical issues
    if (std::isinf(energy) || std::isnan(energy)) return mesh_parameters.MAX_ENERGY;

    return energy;
}

std::pair<double, Eigen::Vector2d> TriWild::get_one_ring_energy(const Tuple& loc) const
{
    auto one_ring = get_one_ring_tris_for_vertex(loc);
    wmtk::DofVector dofx;
    if (is_boundary_vertex(loc) && mesh_parameters.m_boundary_parameter) {
        dofx.resize(1);
        dofx[0] = vertex_attrs[loc.vid(*this)].t; // t
    } else {
        dofx.resize(2); // uv;
        dofx = vertex_attrs[loc.vid(*this)].pos;
    }
    wmtk::NewtonMethodInfo nminfo;
    nminfo.curve_id = vertex_attrs[loc.vid(*this)].curve_id;
    nminfo.target_length = mesh_parameters.m_target_l;
    nminfo.neighbors.resize(one_ring.size(), 4);

    auto is_inverted_coordinates = [this, &loc](auto& A, auto& B) {
        auto res = igl::predicates::orient2d(A, B, this->vertex_attrs[loc.vid(*this)].pos);
        if (res != igl::predicates::Orientation::POSITIVE)
            return true;
        else
            return false;
    };

    for (auto i = 0; i < one_ring.size(); i++) {
        auto tri = one_ring[i];
        assert(!is_inverted(tri));
        auto local_tuples = oriented_tri_vertices(tri);
        for (auto j = 0; j < 3; j++) {
            if (local_tuples[j].vid(*this) == loc.vid(*this)) {
                auto v2 = vertex_attrs[local_tuples[(j + 1) % 3].vid(*this)].pos;
                auto v3 = vertex_attrs[local_tuples[(j + 2) % 3].vid(*this)].pos;
                nminfo.neighbors.row(i) << v2(0), v2(1), v3(0), v3(1);
                assert(!is_inverted_coordinates(v2, v3));
                // sanity check, no inversion should be heres
            }
        }
    }
    assert(one_ring.size() == nminfo.neighbors.rows());
    auto total_energy = 0.;
    Eigen::Vector2d total_gradient;
    total_gradient.setZero(2);
    for (auto i = 0; i < one_ring.size(); i++) {
        // set State
        // pass the state energy
        State state = {};
        state.two_opposite_vertices = nminfo.neighbors.row(i);
        state.dofx = dofx;
        state.scaling = nminfo.target_length;
        if (mesh_parameters.m_boundary_parameter) {
            assert(mesh_parameters.m_boundary.m_arclengths.size() > 0);
            assert(mesh_parameters.m_boundary.m_boundaries.size() > 0);
        }
        DofsToPositions dofs_to_pos(mesh_parameters.m_boundary, nminfo.curve_id);
        mesh_parameters.m_energy->eval(state, dofs_to_pos);
        total_energy += state.value;
        total_gradient += state.gradient;
    }
    return {total_energy, total_gradient};
}

Eigen::VectorXd TriWild::get_quality_all_triangles()
{
    // Use a concurrent vector as for_each_face is parallel
    tbb::concurrent_vector<double> quality;
    quality.reserve(vertex_attrs.size());

    // Evaluate quality in parallel
    for_each_face([&](auto& f) { quality.push_back(get_quality(f)); });

    // Copy back in a VectorXd
    Eigen::VectorXd ret(quality.size());
    for (unsigned i = 0; i < quality.size(); ++i) ret[i] = quality[i];
    return ret;
}

bool TriWild::is_inverted(const Tuple& loc) const
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

void TriWild::mesh_improvement(int max_its)
{
    igl::Timer timer;
    double avg_len = 0.0;
    double pre_avg_len = 0.0;
    double pre_max_energy = -1.0;
    double old_average = 1e-4;
    wmtk::logger().info("target len {}", mesh_parameters.m_target_l);
    wmtk::logger().info("current length {}", avg_edge_len(*this));
    // mesh_parameters.js_log["edge_length_avg_start"] = avg_edge_len(*this);
    for (int it = 0; it < max_its; it++) {
        wmtk::logger().info("\n========it {}========", it);

        ///energy check
        wmtk::logger().info(
            "current max energy {} stop energy {}",
            mesh_parameters.m_max_energy,
            mesh_parameters.m_stop_energy);
        wmtk::logger().info("current length {}", avg_edge_len(*this));

        mesh_parameters.js_log["iteration_" + std::to_string(it)]["num_v"] = vert_capacity();
        mesh_parameters.js_log["iteration_" + std::to_string(it)]["num_f"] = tri_capacity();
        mesh_parameters.js_log["iteration_" + std::to_string(it)]["energy_max"] =
            mesh_parameters.m_max_energy;
        mesh_parameters.js_log["iteration_" + std::to_string(it)]["edge_len_avg"] =
            avg_edge_len(*this);
        mesh_parameters.js_log["iteration_" + std::to_string(it)]["edge_len_target"] =
            mesh_parameters.m_target_l;

        for (auto v : get_vertices()) {
            if (is_boundary_vertex(v)) set_feature(v);
        }
        split_all_edges();
        assert(invariants(get_faces()));
        consolidate_mesh();
        write_displaced_obj(
            "after_split_" + std::to_string(it) + ".obj",
            mesh_parameters.m_triwild_displacement);

        for (auto v : get_vertices()) {
            if (is_boundary_vertex(v)) set_feature(v);
        }
        collapse_all_edges();
        assert(invariants(get_faces()));
        consolidate_mesh();
        write_displaced_obj(
            "after_collapse_" + std::to_string(it) + ".obj",
            mesh_parameters.m_triwild_displacement);

        for (auto v : get_vertices()) {
            if (is_boundary_vertex(v)) set_feature(v);
        }
        swap_all_edges();
        assert(invariants(get_faces()));
        consolidate_mesh();
        write_displaced_obj(
            "after_swap_" + std::to_string(it) + ".obj",
            mesh_parameters.m_triwild_displacement);

        for (auto v : get_vertices()) {
            if (is_boundary_vertex(v)) set_feature(v);
        }
        smooth_all_vertices();
        assert(invariants(get_faces()));
        consolidate_mesh();
        write_displaced_obj(
            "after_smooth_" + std::to_string(it) + ".obj",
            mesh_parameters.m_triwild_displacement);

        auto avg_grad = (mesh_parameters.m_gradient / vert_capacity()).stableNorm();

        wmtk::logger().info(
            "++++++++v {} t {} avg gradient {}++++++++",
            vert_capacity(),
            tri_capacity(),
            mesh_parameters.m_gradient / vert_capacity());
        mesh_parameters.js_log["iteration_" + std::to_string(it)]["avg_grad"] =
            (mesh_parameters.m_gradient / vert_capacity()).stableNorm();
        mesh_parameters.m_gradient = Eigen::Vector2d(0., 0.);
        avg_len = avg_edge_len(*this);
        mesh_parameters.js_log["iteration_" + std::to_string(it)]["edge_len_avg_final"] = avg_len;
        if (abs(avg_grad - old_average) < 1e-5) break;
        old_average = avg_grad;
        if (mesh_parameters.m_target_l <= 0 &&
            mesh_parameters.m_max_energy < mesh_parameters.m_stop_energy) {
            break;
        }

        if (mesh_parameters.m_target_l > 0 &&
            (avg_len - mesh_parameters.m_target_l) * (avg_len - mesh_parameters.m_target_l) <
                1e-4) {
            wmtk::logger().info(
                "doesn't improve edge length. Stopping improvement.\n {} itr finished, max energy "
                "{}, avg length {} ",
                it,
                mesh_parameters.m_max_energy,
                avg_len);
            break;
        }
        if (it > 0 && (mesh_parameters.m_target_l <= 0 &&
                       (pre_max_energy - mesh_parameters.m_stop_energy) *
                               (pre_max_energy - mesh_parameters.m_stop_energy) <
                           1e-2)) {
            wmtk::logger().info(
                "doesn't improve energy. Stopping improvement.\n {} itr finished, max energy {}, "
                "avg length {}",
                it,
                mesh_parameters.m_max_energy,
                avg_len);
            break;
        }
        pre_avg_len = avg_len;
        pre_max_energy = mesh_parameters.m_max_energy;
        consolidate_mesh();
    }

    wmtk::logger().info(
        "/////final: max energy {} , avg len {} ",
        mesh_parameters.m_max_energy,
        avg_len);
    consolidate_mesh();
}
void TriWild::flatten_dofs(Eigen::VectorXd& v_flat)
{
    auto verts = get_vertices();
    v_flat.resize(verts.size() * 2);
    for (auto v : verts) {
        if (is_boundary_vertex(v) && mesh_parameters.m_boundary_parameter) {
            v_flat(v.vid(*this) * 2) = vertex_attrs[v.vid(*this)].t;
            v_flat(v.vid(*this) * 2 + 1) = std::numeric_limits<double>::infinity();
        } else {
            v_flat(v.vid(*this) * 2) = vertex_attrs[v.vid(*this)].pos(0);
            v_flat(v.vid(*this) * 2 + 1) = vertex_attrs[v.vid(*this)].pos(1);
        }
    }
}

// get the energy defined by edge_length_energy over each face of the mesh
// assuming the vert_capacity() == get_vertices.size()
double TriWild::get_mesh_energy(const Eigen::VectorXd& v_flat)
{
    double total_energy = 0;
    int f_cnt = 0;
    for (auto& face : get_faces()) {
        // wmtk::logger().info("getting energy on {} ", f_cnt++);
        auto verts = oriented_tri_vertices(face);
        Eigen::Matrix3d v_matrix;
        v_matrix.setZero(3, 3);
        for (int i = 0; i < 3; i++) {
            auto vert = verts[i];
            if (is_boundary_vertex(vert) && mesh_parameters.m_boundary_parameter) {
                auto uv = mesh_parameters.m_boundary.t_to_uv(
                    vertex_attrs[vert.vid(*this)].curve_id,
                    v_flat[vert.vid(*this) * 2]);
                v_matrix.row(i) = mesh_parameters.m_triwild_displacement(uv(0), uv(1));
            } else {
                auto u = v_flat[vert.vid(*this) * 2];
                auto v = v_flat[vert.vid(*this) * 2 + 1];
                v_matrix.row(i) = mesh_parameters.m_triwild_displacement(u, v);
            }
        }

        assert(mesh_parameters.m_target_l != 0);
        auto BA = v_matrix.row(1) - v_matrix.row(0);
        auto CA = v_matrix.row(2) - v_matrix.row(0);
        auto BC = v_matrix.row(1) - v_matrix.row(2);
        total_energy += pow(BA.squaredNorm() - pow(mesh_parameters.m_target_l, 2), 2);
        total_energy += pow(BC.squaredNorm() - pow(mesh_parameters.m_target_l, 2), 2);
        total_energy += pow(CA.squaredNorm() - pow(mesh_parameters.m_target_l, 2), 2);
        double area = (BA.cross(CA)).squaredNorm();
        double A_hat = 0.5 * (std::sqrt(3) / 2) * 0.5 *
                       pow(mesh_parameters.m_target_l, 2); // this is arbitrary now
        assert(A_hat > 0);
        if (area <= 0) {
            total_energy += std::numeric_limits<double>::infinity();
        }
        if (area < A_hat) {
            assert((area / A_hat) < 1.0);
            total_energy += -(area - A_hat) * (area - A_hat) * log(area / A_hat);
        }
    }
    return total_energy;
}
/// debugging
void TriWild::gradient_debug(int max_its)
{
    // debugging
    // for (auto v : get_vertices()) {
    //     if (is_boundary_vertex(v)) set_feature(v);
    // }
    split_all_edges();
    assert(invariants(get_faces()));
    consolidate_mesh();
    write_obj("before_smooth.obj");
    double old_average = 1e-4;
    for (int it = 0; it < max_its; it++) {
        wmtk::logger().info("\n========it {}========", it);
        wmtk::logger().info("current length {}", avg_edge_len(*this));
        // for (auto v : get_vertices()) {
        //     if (is_boundary_vertex(v)) set_feature(v);
        // }
        smooth_all_vertices();
        assert(invariants(get_faces()));
        consolidate_mesh();
        write_displaced_obj(
            "smooth_" + std::to_string(it) + ".obj",
            mesh_parameters.m_triwild_displacement);

        wmtk::logger().info(
            "++++++++v {} t {} avg gradient {}++++++++",
            vert_capacity(),
            tri_capacity(),
            mesh_parameters.m_gradient / vert_capacity());
        auto avg_grad = (mesh_parameters.m_gradient / vert_capacity()).stableNorm();

        mesh_parameters.js_log["iteration_" + std::to_string(it)]["avg_grad"] = avg_grad;
        if (abs(avg_grad - old_average) < 1e-5) break;
        old_average = avg_grad;
        mesh_parameters.m_gradient = Eigen::Vector2d(0., 0.);
    }
}
} // namespace triwild