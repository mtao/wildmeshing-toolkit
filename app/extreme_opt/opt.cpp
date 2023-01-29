
#include "ExtremeOpt.h"
#include "wmtk/ExecutionScheduler.hpp"

#include <Eigen/src/Core/util/Constants.h>
#include <igl/Timer.h>
#include <igl/cat.h>
#include <igl/grad.h>
#include <igl/local_basis.h>
#include <wmtk/utils/AMIPS2D.h>
#include <Eigen/Sparse>
#include <array>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/TriQualityUtils.hpp>

#include <igl/boundary_loop.h>
#include <igl/predicates/predicates.h>
#include <igl/upsample.h>
#include <igl/writeOBJ.h>
#include <limits>
#include <optional>
#include <wmtk/utils/TupleUtils.hpp>
#include "SYMDIR.h"


void buildAeq(
    const Eigen::MatrixXi& EE,
    const Eigen::MatrixXd& uv,
    const Eigen::MatrixXi& F,
    Eigen::SparseMatrix<double>& Aeq)
{
    int N = uv.rows();
    int c = 0;
    int m = EE.rows() / 2;

    std::vector<std::vector<int>> bds;
    igl::boundary_loop(F, bds);

    int n_fix_dof = 3;

    std::set<std::pair<int, int>> added_e;

    Aeq.resize(2 * m + n_fix_dof, uv.rows() * 2);
    int A2, B2, C2, D2;
    for (int i = 0; i < EE.rows(); i++) {
        int A2 = EE(i, 0);
        int B2 = EE(i, 1);
        int C2 = EE(i, 2);
        int D2 = EE(i, 3);
        auto e0 = std::make_pair(A2, B2);
        auto e1 = std::make_pair(C2, D2);
        if (added_e.find(e0) != added_e.end() || added_e.find(e1) != added_e.end()) continue;
        added_e.insert(e0);
        added_e.insert(e1);

        Eigen::Matrix<double, 2, 1> e_ab = uv.row(B2) - uv.row(A2);
        Eigen::Matrix<double, 2, 1> e_dc = uv.row(C2) - uv.row(D2);

        Eigen::Matrix<double, 2, 1> e_ab_perp;
        e_ab_perp(0) = -e_ab(1);
        e_ab_perp(1) = e_ab(0);
        double angle = atan2(-e_ab_perp.dot(e_dc), e_ab.dot(e_dc));
        int r = (int)(round(2 * angle / double(igl::PI)) + 2) % 4;

        std::vector<Eigen::Matrix<double, 2, 2>> r_mat(4);
        r_mat[0] << -1, 0, 0, -1;
        r_mat[1] << 0, 1, -1, 0;
        r_mat[2] << 1, 0, 0, 1;
        r_mat[3] << 0, -1, 1, 0;

        Aeq.coeffRef(c, A2) += 1;
        Aeq.coeffRef(c, B2) += -1;
        Aeq.coeffRef(c + 1, A2 + N) += 1;
        Aeq.coeffRef(c + 1, B2 + N) += -1;

        Aeq.coeffRef(c, C2) += r_mat[r](0, 0);
        Aeq.coeffRef(c, D2) += -r_mat[r](0, 0);
        Aeq.coeffRef(c, C2 + N) += r_mat[r](0, 1);
        Aeq.coeffRef(c, D2 + N) += -r_mat[r](0, 1);
        Aeq.coeffRef(c + 1, C2) += r_mat[r](1, 0);
        Aeq.coeffRef(c + 1, D2) += -r_mat[r](1, 0);
        Aeq.coeffRef(c + 1, C2 + N) += r_mat[r](1, 1);
        Aeq.coeffRef(c + 1, D2 + N) += -r_mat[r](1, 1);
        c = c + 2;
    }

    double min_u_diff = 1e10;
    int min_u_diff_id = 0;
    auto l = bds[0];
    for (int i = 0; i < l.size(); i++) {
        double u_diff = abs(uv(l[i], 0) - uv(l[(i + 1) % l.size()], 0));
        if (u_diff < min_u_diff) {
            min_u_diff = u_diff;
            min_u_diff_id = i;
        }
    }

    std::cout << "fix " << l[min_u_diff_id] << std::endl;
    Aeq.coeffRef(c, l[min_u_diff_id]) = 1;
    Aeq.coeffRef(c + 1, l[min_u_diff_id] + N) = 1;
    c = c + 2;
    std::cout << "fix " << l[(min_u_diff_id + 1) % l.size()] << std::endl;
    Aeq.coeffRef(c, l[(min_u_diff_id + 1) % l.size()]) = 1;
    c = c + 1;
}

void buildkkt(
    Eigen::SparseMatrix<double>& hessian,
    Eigen::SparseMatrix<double>& Aeq,
    Eigen::SparseMatrix<double>& AeqT,
    Eigen::SparseMatrix<double>& kkt)
{
    kkt.reserve(hessian.nonZeros() + Aeq.nonZeros() + AeqT.nonZeros());
    for (Eigen::Index c = 0; c < kkt.cols(); ++c) {
        kkt.startVec(c);
        if (c < hessian.cols()) {
            for (typename Eigen::SparseMatrix<double>::InnerIterator ithessian(hessian, c);
                 ithessian;
                 ++ithessian)
                kkt.insertBack(ithessian.row(), c) = ithessian.value();
            for (typename Eigen::SparseMatrix<double>::InnerIterator itAeq(Aeq, c); itAeq; ++itAeq)
                kkt.insertBack(itAeq.row() + hessian.rows(), c) = itAeq.value();
        } else {
            for (typename Eigen::SparseMatrix<double>::InnerIterator itAeqT(
                     AeqT,
                     c - hessian.cols());
                 itAeqT;
                 ++itAeqT)
                kkt.insertBack(itAeqT.row(), c) = itAeqT.value();
        }
    }
    kkt.finalize();
}

int check_flip(const Eigen::MatrixXd& uv, const Eigen::MatrixXi& Fn)
{
    int fl = 0;
    for (int i = 0; i < Fn.rows(); i++) {
        Eigen::Matrix<double, 1, 2> a_db(uv(Fn(i, 0), 0), uv(Fn(i, 0), 1));
        Eigen::Matrix<double, 1, 2> b_db(uv(Fn(i, 1), 0), uv(Fn(i, 1), 1));
        Eigen::Matrix<double, 1, 2> c_db(uv(Fn(i, 2), 0), uv(Fn(i, 2), 1));
        if (igl::predicates::orient2d(a_db, b_db, c_db) != igl::predicates::Orientation::POSITIVE) {
            fl++;
        }
    }
    return fl;
}

using namespace wmtk;
auto renew = [](auto& m, auto op, auto& tris) {
    auto edges = m.new_edges_after(tris);
    auto optup = std::vector<std::pair<std::string, wmtk::TriMesh::Tuple>>();
    for (auto& e : edges) optup.emplace_back(op, e);
    return optup;
};

auto renew_collapse = [](auto& m, auto op, auto& tris) {
    auto edges = m.new_edges_after(tris);
    auto optup = std::vector<std::pair<std::string, wmtk::TriMesh::Tuple>>();
    for (auto& e : edges) {
        if (m.is_boundary_edge(e)) {
            optup.emplace_back("test_op", e);
        } else {
            optup.emplace_back("edge_collapse", e);
        }
    }
    return optup;
};


namespace extremeopt {
void get_grad_op(Eigen::MatrixXd& V, const Eigen::MatrixXi& F, Eigen::SparseMatrix<double>& grad_op)
{
    Eigen::MatrixXd F1, F2, F3;
    igl::local_basis(V, F, F1, F2, F3);

    Eigen::SparseMatrix<double> G;
    igl::grad(V, F, G, false);

    auto face_proj = [](Eigen::MatrixXd& F) {
        std::vector<Eigen::Triplet<double>> IJV;
        int f_num = F.rows();
        for (int i = 0; i < F.rows(); i++) {
            IJV.push_back(Eigen::Triplet<double>(i, i, F(i, 0)));
            IJV.push_back(Eigen::Triplet<double>(i, i + f_num, F(i, 1)));
            IJV.push_back(Eigen::Triplet<double>(i, i + 2 * f_num, F(i, 2)));
        }
        Eigen::SparseMatrix<double> P(f_num, 3 * f_num);
        P.setFromTriplets(IJV.begin(), IJV.end());
        return P;
    };

    Eigen::SparseMatrix<double> Dx = face_proj(F1) * G;
    Eigen::SparseMatrix<double> Dy = face_proj(F2) * G;

    Eigen::SparseMatrix<double> hstack = igl::cat(1, Dx, Dy);
    Eigen::SparseMatrix<double> empty(hstack.rows(), hstack.cols());

    grad_op = igl::cat(1, igl::cat(2, hstack, empty), igl::cat(2, empty, hstack));
}
} // namespace extremeopt

double extremeopt::ExtremeOpt::get_e_max_onering(const Tuple& t)
{
    Eigen::MatrixXd V_local, uv_local, Ji;
    Eigen::MatrixXi F_local;
    Eigen::SparseMatrix<double> G_local;
    get_mesh_onering(t, V_local, uv_local, F_local);
    get_grad_op(V_local, F_local, G_local);
    wmtk::jacobian_from_uv(G_local, uv_local, Ji);
    return wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3)).maxCoeff();
}

void extremeopt::ExtremeOpt::cache_edge_positions(const Tuple& t)
{
    position_cache.local().V1 = vertex_attrs[t.vid(*this)].pos_3d;
    position_cache.local().V2 = vertex_attrs[t.switch_vertex(*this).vid(*this)].pos_3d;
    position_cache.local().uv1 = vertex_attrs[t.vid(*this)].pos;
    position_cache.local().uv2 = vertex_attrs[t.switch_vertex(*this).vid(*this)].pos;

    position_cache.local().vid1 = t.vid(*this);
    position_cache.local().vid2 = t.switch_vertex(*this).vid(*this);

    position_cache.local().is_v1_bd = this->is_boundary_vertex(t);
    position_cache.local().is_v2_bd = this->is_boundary_vertex(t.switch_vertex(*this));

    if (position_cache.local().is_v1_bd) {
        auto onering_e = get_one_ring_edges_for_vertex(t);
        int cnt = 0;
        for (auto e : onering_e) {
            if (is_boundary_edge(e)) {
                if (cnt == 0) {
                    if (e.is_ccw(*this))
                        position_cache.local().bd_e1 = e;
                    else
                        position_cache.local().bd_e1 = e.switch_vertex(*this);
                } else {
                    if (e.is_ccw(*this))
                        position_cache.local().bd_e2 = e;
                    else
                        position_cache.local().bd_e2 = e.switch_vertex(*this);
                }
                cnt++;
            }
            if (cnt == 2) break;
        }
    } else if (position_cache.local().is_v2_bd) {
        auto onering_e = get_one_ring_edges_for_vertex(t.switch_vertex(*this));
        int cnt = 0;
        for (auto e : onering_e) {
            if (is_boundary_edge(e)) {
                if (cnt == 0) {
                    if (e.is_ccw(*this))
                        position_cache.local().bd_e1 = e;
                    else
                        position_cache.local().bd_e1 = e.switch_vertex(*this);
                } else {
                    if (e.is_ccw(*this))
                        position_cache.local().bd_e2 = e;
                    else
                        position_cache.local().bd_e2 = e.switch_vertex(*this);
                }
                cnt++;
            }
            if (cnt == 2) break;
        }
    }
    double E1, E2;
    E1 = get_e_max_onering(t);
    E2 = get_e_max_onering(t.switch_vertex(*this));

    position_cache.local().E_max_before_collpase = std::max(E1, E2);
}

bool extremeopt::ExtremeOpt::split_edge_before(const Tuple& t)
{
    if (!t.is_valid(*this)) {
        return false;
    }
    if (!TriMesh::split_edge_before(t)) {
        return false;
    }


    if (is_boundary_edge(t)) {
        return false;
    }

    position_cache.local().V1 = vertex_attrs[t.vid(*this)].pos_3d;
    position_cache.local().V2 = vertex_attrs[t.switch_vertex(*this).vid(*this)].pos_3d;
    position_cache.local().uv1 = vertex_attrs[t.vid(*this)].pos;
    position_cache.local().uv2 = vertex_attrs[t.switch_vertex(*this).vid(*this)].pos;


    return true;
}

bool extremeopt::ExtremeOpt::split_edge_after(const Tuple& t)
{
    Eigen::Vector3d V = (position_cache.local().V1 + position_cache.local().V2) / 2.0;
    Eigen::Vector2d uv = (position_cache.local().uv1 + position_cache.local().uv2) / 2.0;
    auto vid = t.switch_vertex(*this).vid(*this);
    // auto vid = t.vid(*this);

    // std::cout << uv << std::endl;
    // std::cout << V << std::endl;

    auto& v = vertex_attrs[vid];
    // if((v.pos.array() != 0).all() ||
    //(v.pos_3d.array() != 0).all()) {
    //     spdlog::error("writing to a nontrivial vertex after a split, should be a new one! {} ::
    //     {}",
    //             fmt::join(v.pos,","),
    //             fmt::join(v.pos_3d,",")
    //             );
    // }
    v.pos = uv;
    v.pos_3d = V;
    // for(int j = 0; j <
    // auto t = oriented_tri_vertices(t);

    return true;
}

void extremeopt::ExtremeOpt::split_all_edges()
{
    Eigen::MatrixXi EE;
    export_EE(EE);
    size_t vid_threshold = 0;
    auto collect_all_ops_split = std::vector<std::pair<std::string, Tuple>>();

    for (auto& loc : get_faces()) {
        auto t0 = loc;
        auto t1 = t0.switch_edge(*this);
        auto t2 = t0.switch_vertex(*this).switch_edge(*this);
        double l0 =
            (vertex_attrs[t0.vid(*this)].pos - vertex_attrs[t0.switch_vertex(*this).vid(*this)].pos)
                .norm();
        double l1 =
            (vertex_attrs[t1.vid(*this)].pos - vertex_attrs[t1.switch_vertex(*this).vid(*this)].pos)
                .norm();
        double l2 =
            (vertex_attrs[t2.vid(*this)].pos - vertex_attrs[t2.switch_vertex(*this).vid(*this)].pos)
                .norm();
        if (is_boundary_edge(t0)) l0 = 0;
        if (is_boundary_edge(t1)) l1 = 0;
        if (is_boundary_edge(t2)) l2 = 0;

        if (l0 >= l1 && l0 >= l2) {
            collect_all_ops_split.emplace_back("edge_split", t0);
        } else if (l1 >= l2) {
            collect_all_ops_split.emplace_back("edge_split", t1);

        } else {
            collect_all_ops_split.emplace_back("edge_split", t2);
        }
    }

    auto setup_and_execute = [&](auto& executor_split) {
        // vid_threshold = vert_capacity();
        // executor_split.renew_neighbor_tuples =
        //     [&](auto& m, auto op, auto& tris) {
        //     auto edges = m.replace_edges_after_split(tris, vid_threshold);
        //     auto optup = std::vector<std::pair<std::string, TriMesh::Tuple>>();
        //     for (auto& e : edges) optup.emplace_back(op, e);
        //     return optup;
        // };

        executor_split.num_threads = NUM_THREADS;
        executor_split(*this, collect_all_ops_split);
    };
    auto executor_split = wmtk::ExecutePass<ExtremeOpt, wmtk::ExecutionPolicy::kSeq>();
    setup_and_execute(executor_split);

    update_constraints_EE_v(EE);
}

bool extremeopt::ExtremeOpt::collapse_edge_before(const Tuple& t)
{
    if (!t.is_valid(*this)) {
        return false;
    }
    if (!wmtk::TriMesh::collapse_edge_before(t)) {
        return false;
    }

    // avoid boundary edges here
    if (is_boundary_vertex(t) && is_boundary_vertex(t.switch_vertex(*this))) {
        return false;
    }
    cache_edge_positions(t);
    return true;
}

bool extremeopt::ExtremeOpt::collapse_edge_after(const Tuple& t)
{
    // const Eigen::Vector3d V = (position_cache.local().V1 + position_cache.local().V2) / 2.0;
    // const Eigen::Vector2d uv = (position_cache.local().uv1 + position_cache.local().uv2) / 2.0;
    Eigen::Vector3d V;
    Eigen::Vector2d uv;
    if (position_cache.local().is_v1_bd) {
        V = position_cache.local().V1;
        uv = position_cache.local().uv1;
    } else {
        V = position_cache.local().V2;
        uv = position_cache.local().uv2;
    }
    // const Eigen::Vector3d V = position_cache.local().V1;
    // const Eigen::Vector2d uv = position_cache.local().uv1;

    auto vid = t.vid(*this);
    vertex_attrs[vid].pos_3d = V;
    vertex_attrs[vid].pos = uv;

    // get local F,V,uv
    auto vid_onering = get_one_ring_vids_for_vertex(vid);
    auto locs = get_one_ring_tris_for_vertex(t);
    Eigen::MatrixXd V_local(vid_onering.size(), 3);
    Eigen::MatrixXd uv_local(vid_onering.size(), 2);
    for (size_t i = 0; i < vid_onering.size(); i++) {
        V_local.row(i) = vertex_attrs[vid_onering[i]].pos_3d;
        uv_local.row(i) = vertex_attrs[vid_onering[i]].pos;
    }
    std::vector<int> v_map(vertex_attrs.size(), -1);
    for (size_t i = 0; i < vid_onering.size(); i++) {
        v_map[vid_onering[i]] = i;
    }
    Eigen::MatrixXi F_local(locs.size(), 3);
    for (size_t i = 0; i < locs.size(); i++) {
        int t_id = locs[i].fid(*this);
        auto local_tuples = oriented_tri_vertices(locs[i]);
        for (size_t j = 0; j < 3; j++) {
            F_local(i, j) = v_map[local_tuples[j].vid(*this)];
        }
    }
    Eigen::VectorXd area_local_3d, area_local;
    igl::doublearea(V_local, F_local, area_local_3d);
    igl::doublearea(uv_local, F_local, area_local);


    // export_mesh(V_local, F_local, uv_local);
    // first check flips
    if (area_local_3d.minCoeff() <= 0 || area_local.minCoeff() <= 0) {
        // std::cout << "collapse causing flips" << std::endl;
        return false;
    }

    Eigen::SparseMatrix<double> G_local;
    get_grad_op(V_local, F_local, G_local);

    double E_max_after_collapse;
    Eigen::MatrixXd Ji;
    wmtk::jacobian_from_uv(G_local, uv_local, Ji);
    E_max_after_collapse =
        wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3)).maxCoeff();

    if (E_max_after_collapse > position_cache.local().E_max_before_collpase) {
        // E_max does not go down
        return false;
    }

    // std::cout << "collapse succeed" << std::endl;

    // update constraints
    if (this->is_boundary_vertex(t)) {
        // std::cout << "update constraints around vertex " << t.vid(*this) << std::endl;

        auto one_ring_edges = this->get_one_ring_edges_for_vertex(t);
        std::vector<Tuple> two_bd_e;
        Tuple e_old_l, e_old_r;
        Tuple e_new_l, e_new_r;
        for (auto e : one_ring_edges) {
            if (this->is_boundary_edge(e)) {
                Tuple candidate_e = e;
                if (!candidate_e.is_ccw(*this)) {
                    candidate_e = candidate_e.switch_vertex(*this);
                }
                if (candidate_e.vid(*this) == t.vid(*this)) {
                    e_new_r = candidate_e;
                } else {
                    e_new_l = candidate_e;
                }
            }
        }
        if (position_cache.local().bd_e1.vid(*this) == position_cache.local().vid1 ||
            position_cache.local().bd_e1.vid(*this) == position_cache.local().vid2) {
            e_old_r = position_cache.local().bd_e1;
            e_old_l = position_cache.local().bd_e2;
        } else {
            e_old_r = position_cache.local().bd_e2;
            e_old_l = position_cache.local().bd_e1;
        }

        auto e_old_r_pair = edge_attrs[e_old_r.eid_unsafe(*this)].pair;
        auto e_old_l_pair = edge_attrs[e_old_l.eid_unsafe(*this)].pair;

        if (edge_attrs[e_old_r.eid_unsafe(*this)].pair.eid_unsafe(*this) ==
            e_old_l.eid_unsafe(*this)) {
            // std::cout << "in this case the two new bd edges is a pair" << std::endl;
            edge_attrs[e_new_r.eid(*this)].pair = e_new_l;
            edge_attrs[e_new_l.eid(*this)].pair = e_new_r;
        } else {
            edge_attrs[e_old_r_pair.eid_unsafe(*this)].pair = e_new_r;
            edge_attrs[e_old_l_pair.eid_unsafe(*this)].pair = e_new_l;
            edge_attrs[e_new_r.eid(*this)].pair = e_old_r_pair;
            edge_attrs[e_new_l.eid(*this)].pair = e_old_l_pair;
        }
    }


    auto one_ring_tris = get_one_ring_tris_for_vertex(t);

    for (auto t_tmp : one_ring_tris) {
        Tuple t0 = t_tmp;
        Tuple t1 = t0.switch_edge(*this);
        Tuple t2 = t1.switch_vertex(*this).switch_edge(*this);
        if (!t0.is_ccw(*this)) t0 = t0.switch_vertex(*this);
        if (!t1.is_ccw(*this)) t1 = t1.switch_vertex(*this);
        if (!t2.is_ccw(*this)) t2 = t2.switch_vertex(*this);

        if (is_boundary_edge(t0)) {
            edge_attrs[edge_attrs[t0.eid(*this)].pair.eid_unsafe(*this)].pair = t0;
        }
        if (is_boundary_edge(t1)) {
            edge_attrs[edge_attrs[t1.eid(*this)].pair.eid_unsafe(*this)].pair = t1;
        }
        if (is_boundary_edge(t2)) {
            edge_attrs[edge_attrs[t2.eid(*this)].pair.eid_unsafe(*this)].pair = t2;
        }
    }

    return true;
}

bool extremeopt::ExtremeOpt::collapse_bd_edge_after(
    const Tuple& t,
    const Eigen::Vector3d& V_keep,
    const Eigen::Vector2d& uv_keep,
    Tuple& t_l_old,
    Tuple& t_r_old,
    double& E_max)
{
    // update vertex position
    auto vid = t.vid(*this);
    vertex_attrs[vid].pos_3d = V_keep;
    vertex_attrs[vid].pos = uv_keep;

    // get local mesh and check area/E_max
    Eigen::MatrixXd V_local, uv_local;
    Eigen::MatrixXi F_local;
    get_mesh_onering(t, V_local, uv_local, F_local);
    Eigen::VectorXd area_local_3d, area_local;
    igl::doublearea(V_local, F_local, area_local_3d);
    igl::doublearea(uv_local, F_local, area_local);
    if (area_local_3d.minCoeff() <= 0 || area_local.minCoeff() <= 0) {
        return false;
    }
    Eigen::SparseMatrix<double> G_local;
    get_grad_op(V_local, F_local, G_local);
    Eigen::MatrixXd Ji;
    wmtk::jacobian_from_uv(G_local, uv_local, Ji);
    E_max = wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3)).maxCoeff();

    // update constraints
    auto one_ring_edges = this->get_one_ring_edges_for_vertex(t);
    Tuple e_new_l, e_new_r;
    for (auto e : one_ring_edges) {
        if (this->is_boundary_edge(e)) {
            Tuple candidate_e = e;
            if (!candidate_e.is_ccw(*this)) {
                candidate_e = candidate_e.switch_vertex(*this);
            }
            if (candidate_e.vid(*this) == t.vid(*this)) {
                e_new_r = candidate_e;
            } else {
                e_new_l = candidate_e;
            }
        }
    }
    auto e_old_l_pair = edge_attrs[t_l_old.eid_unsafe(*this)].pair;
    auto e_old_r_pair = edge_attrs[t_r_old.eid_unsafe(*this)].pair;
    edge_attrs[e_old_r_pair.eid_unsafe(*this)].pair = e_new_r;
    edge_attrs[e_old_l_pair.eid_unsafe(*this)].pair = e_new_l;
    edge_attrs[e_new_r.eid(*this)].pair = e_old_r_pair;
    edge_attrs[e_new_l.eid(*this)].pair = e_old_l_pair;

    auto one_ring_tris = get_one_ring_tris_for_vertex(t);
    for (auto t_tmp : one_ring_tris) {
        Tuple t0 = t_tmp;
        Tuple t1 = t0.switch_edge(*this);
        Tuple t2 = t1.switch_vertex(*this).switch_edge(*this);
        if (!t0.is_ccw(*this)) t0 = t0.switch_vertex(*this);
        if (!t1.is_ccw(*this)) t1 = t1.switch_vertex(*this);
        if (!t2.is_ccw(*this)) t2 = t2.switch_vertex(*this);

        if (is_boundary_edge(t0)) {
            edge_attrs[edge_attrs[t0.eid(*this)].pair.eid_unsafe(*this)].pair = t0;
        }
        if (is_boundary_edge(t1)) {
            edge_attrs[edge_attrs[t1.eid(*this)].pair.eid_unsafe(*this)].pair = t1;
        }
        if (is_boundary_edge(t2)) {
            edge_attrs[edge_attrs[t2.eid(*this)].pair.eid_unsafe(*this)].pair = t2;
        }
    }


    return true;
}

bool extremeopt::ExtremeOpt::swap_edge_before(const Tuple& t)
{
    if (!t.is_valid(*this)) return false;
    if (!TriMesh::swap_edge_before(t)) return false;
    // if (vertex_attrs[t.vid(*this)].fixed &&
    // vertex_attrs[t.switch_vertex(*this).vid(*this)].fixed) return false;
    Tuple t1 = t.switch_vertex(*this).switch_edge(*this);
    Tuple t2 = t.switch_face(*this).value().switch_edge(*this);
    if (!t1.is_ccw(*this)) t1 = t1.switch_vertex(*this);
    if (!t2.is_ccw(*this)) t2 = t2.switch_vertex(*this);
    swap_cache.local() = std::make_pair(t1, t2);
    return true;
}

std::vector<wmtk::TriMesh::Tuple> extremeopt::ExtremeOpt::new_edges_after(
    const std::vector<wmtk::TriMesh::Tuple>& tris) const
{
    std::vector<wmtk::TriMesh::Tuple> new_edges;

    for (auto t : tris) {
        for (auto j = 0; j < 3; j++) {
            new_edges.push_back(wmtk::TriMesh::tuple_from_edge(t.fid(*this), j));
        }
    }
    wmtk::unique_edge_tuples(*this, new_edges);
    return new_edges;
}

std::vector<wmtk::TriMesh::Tuple> extremeopt::ExtremeOpt::replace_edges_after_split(
    const std::vector<wmtk::TriMesh::Tuple>& tris,
    const size_t vid_threshold) const
{
    std::vector<wmtk::TriMesh::Tuple> new_edges;

    for (auto t : tris) {
        auto tmptup = (t.switch_vertex(*this)).switch_edge(*this);
        if (tmptup.vid(*this) < vid_threshold &&
            (tmptup.switch_vertex(*this)).vid(*this) < vid_threshold)
            new_edges.push_back(tmptup);
    }
    wmtk::unique_edge_tuples(*this, new_edges);
    return new_edges;
}

bool extremeopt::ExtremeOpt::swap_edge_after(const Tuple& t)
{
    // std::cout << "connectivity test:" << std::endl;
    auto tri1 = oriented_tri_vids(t);
    auto t_opp = t.switch_face(*this);
    auto tri2 = oriented_tri_vids(t_opp.value());

    // std::cout << "trinagle 1 : ";
    // for (auto i : tri1) std::cout << i << " ";
    // std::cout << std::endl << "triangle 2 : ";
    // for (auto i : tri2) std::cout << i << " ";
    // std::cout << std::endl;

    std::vector<int> v_ids;
    Eigen::MatrixXi F_local(2, 3);
    Eigen::MatrixXd V_local(4, 3), uv_local(4, 2);
    for (int i = 0; i < 3; i++) {
        v_ids.push_back(tri1[i]);
        F_local(0, i) = i;
    }
    for (int i = 0; i < 3; i++) {
        int j = 0;
        while (j < 3 && v_ids[j] != (int)tri2[i]) {
            j++;
        }
        if (j == 3) v_ids.push_back((int)tri2[i]);
        F_local(1, i) = j;
    }
    assert(v_ids.size() == 4);

    for (int i = 0; i < 4; i++) {
        V_local.row(i) = vertex_attrs[v_ids[i]].pos_3d;
        uv_local.row(i) = vertex_attrs[v_ids[i]].pos;
    }

    Eigen::VectorXd dblarea, dblarea_3d;
    igl::doublearea(V_local, F_local, dblarea_3d);
    igl::doublearea(uv_local, F_local, dblarea);


    if (dblarea_3d.minCoeff() <= 0.0) {
        // std::cout << "zero 3d area: " << dblarea_3d.minCoeff() << std::endl;
        return false;
    }
    if (dblarea.minCoeff() <= 0.0) {
        // std::cout << "zero/negative 2d area: " << dblarea.minCoeff() << std::endl;
        return false;
    }


    // TODO: compute and compare local energy
    // get the F_local_before swapping
    // get dblareas, then compute the energy
    Eigen::MatrixXi F_local_old(2, 3);
    int v_to_change = 0;
    while (F_local(1, 0) != v_to_change && F_local(1, 1) != v_to_change &&
           F_local(1, 2) != v_to_change) {
        v_to_change++;
    }
    // std::cout << "change: " << v_to_change << "->3" << std::endl;
    // std::cout << "change: " << (v_to_change + 1) % 3 << "<->" << (v_to_change + 2) % 3 <<
    // std::endl;

    for (int i = 0; i < 3; i++) {
        if (F_local(0, i) == v_to_change) {
            F_local_old(0, i) = 3;
        } else {
            F_local_old(0, i) = F_local(0, i);
        }
    }

    for (int i = 0; i < 3; i++) {
        if (F_local(1, i) == (v_to_change + 1) % 3) {
            F_local_old(1, i) = (v_to_change + 2) % 3;
        } else if (F_local(1, i) == (v_to_change + 2) % 3) {
            F_local_old(1, i) = (v_to_change + 1) % 3;
        } else {
            F_local_old(1, i) = F_local(1, i);
        }
    }

    // std::cout << "F_local after swap:\n" << F_local << std::endl;
    // std::cout << "F_local before swap:\n" << F_local_old << std::endl;

    Eigen::VectorXd dblarea_3d_old;
    igl::doublearea(V_local, F_local_old, dblarea_3d_old);

    Eigen::SparseMatrix<double> G_local, G_local_old;
    get_grad_op(V_local, F_local, G_local);
    get_grad_op(V_local, F_local_old, G_local_old);

    double E, E_old;
    Eigen::MatrixXd Ji;
    wmtk::jacobian_from_uv(G_local, uv_local, Ji);
    Eigen::MatrixXd Es =
        wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3));
    E = Es.maxCoeff(); // compute E_max
    if (std::isnan(Es(0)) || std::isnan(Es(1))) {
        return false;
    }
    if (Es.minCoeff() <= 0) {
        return false;
    }
    // E = wmtk::compute_energy_from_jacobian(Ji, dblarea_3d) * dblarea_3d.sum(); // compute E_sum

    Eigen::MatrixXd Ji_old;
    wmtk::jacobian_from_uv(G_local_old, uv_local, Ji);


    E_old = wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3))
                .maxCoeff(); // compute E_max
    // E_old = wmtk::compute_energy_from_jacobian(Ji, dblarea_3d_old) * dblarea_3d_old.sum(); // compute E_sum


    if (E >= E_old) {
        // std::cout << "energy increase after swapping" << std::endl;
        return false;
    }

    // update constraints after collapse
    Tuple t1 = t.switch_edge(*this);
    Tuple t2 = t.switch_face(*this).value().switch_vertex(*this).switch_edge(*this);
    Tuple t1_old = swap_cache.local().first;
    Tuple t2_old = swap_cache.local().second;
    if (!t1.is_ccw(*this)) t1 = t1.switch_vertex(*this);
    if (!t2.is_ccw(*this)) t2 = t2.switch_vertex(*this);
    bool flag = true;
    if (is_boundary_edge(t1)) {
        auto t1_old_pair = edge_attrs[t1_old.eid_unsafe(*this)].pair;
        if (t1_old_pair.eid_unsafe(*this) == t2_old.eid_unsafe(*this)) {
            flag = false;
            edge_attrs[t1.eid(*this)].pair = t2;
            edge_attrs[t2.eid(*this)].pair = t1;
        } else {
            edge_attrs[t1_old_pair.eid_unsafe(*this)].pair = t1;
            edge_attrs[t1.eid(*this)].pair = t1_old_pair;
        }
    }
    if (flag && is_boundary_edge(t2)) {
        auto t2_old_pair = edge_attrs[t2_old.eid_unsafe(*this)].pair;
        edge_attrs[t2_old_pair.eid_unsafe(*this)].pair = t2;
        edge_attrs[t2.eid(*this)].pair = t2_old_pair;
    }

    auto one_ring_tris = get_one_ring_tris_for_vertex(t);

    for (auto t_tmp : one_ring_tris) {
        Tuple t0 = t_tmp;
        Tuple t1 = t0.switch_edge(*this);
        Tuple t2 = t1.switch_vertex(*this).switch_edge(*this);
        if (!t0.is_ccw(*this)) t0 = t0.switch_vertex(*this);
        if (!t1.is_ccw(*this)) t1 = t1.switch_vertex(*this);
        if (!t2.is_ccw(*this)) t2 = t2.switch_vertex(*this);

        if (is_boundary_edge(t0)) {
            edge_attrs[edge_attrs[t0.eid(*this)].pair.eid_unsafe(*this)].pair = t0;
        }
        if (is_boundary_edge(t1)) {
            edge_attrs[edge_attrs[t1.eid(*this)].pair.eid_unsafe(*this)].pair = t1;
        }
        if (is_boundary_edge(t2)) {
            edge_attrs[edge_attrs[t2.eid(*this)].pair.eid_unsafe(*this)].pair = t2;
        }
    }

    return true;
}

bool extremeopt::ExtremeOpt::smooth_before(const Tuple& t)
{
    if (!t.is_valid(*this)) {
        std::cout << "tuple not valid" << std::endl;
        return false;
    }

    if (is_boundary_vertex(t)) {
        return false;
    }
    // // it's okay to move the boundary(for now)
    // if (vertex_attrs[t.vid(*this)].fixed)
    //     return false;
    return true;
}


bool extremeopt::ExtremeOpt::smooth_after(const Tuple& t)
{
    // Newton iterations are encapsulated here.
    wmtk::logger().trace("Newton iteration for vertex smoothing.");
    auto vid = t.vid(*this);

    wmtk::logger().trace("smooth vertex {}", vid);
    // std::cout << "vertex " << vid << std::endl;

    auto vid_onering = get_one_ring_vids_for_vertex(vid);
    auto locs = get_one_ring_tris_for_vertex(t);
    assert(locs.size() > 0);

    // get local (V, F)
    Eigen::MatrixXd V_local(vid_onering.size(), 3);
    Eigen::MatrixXd uv_local(vid_onering.size(), 2);
    for (size_t i = 0; i < vid_onering.size(); i++) {
        V_local.row(i) = vertex_attrs[vid_onering[i]].pos_3d;
        uv_local.row(i) = vertex_attrs[vid_onering[i]].pos;
    }
    std::vector<int> v_map(vertex_attrs.size(), -1);
    for (size_t i = 0; i < vid_onering.size(); i++) {
        v_map[vid_onering[i]] = i;
    }
    Eigen::MatrixXi F_local(locs.size(), 3);
    Eigen::VectorXd area_local(locs.size());
    for (size_t i = 0; i < locs.size(); i++) {
        int t_id = locs[i].fid(*this);
        auto local_tuples = oriented_tri_vertices(locs[i]);
        for (size_t j = 0; j < 3; j++) {
            F_local(i, j) = v_map[local_tuples[j].vid(*this)];
        }
        // area_local(i) = face_attrs[t_id].area_3d;
    }
    igl::doublearea(V_local, F_local, area_local);
    Eigen::SparseMatrix<double> G_local;
    get_grad_op(V_local, F_local, G_local);
    // std::cout << "G_local: \n" << G_local << std::endl;

    auto compute_energy = [&G_local, &area_local](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_local, aaa, Ji);
        return wmtk::compute_energy_from_jacobian(Ji, area_local);
    };


    Eigen::SparseMatrix<double> hessian_local;
    Eigen::VectorXd grad_local;

    double local_energy_0 = wmtk::get_grad_and_hessian(
        G_local,
        area_local,
        uv_local,
        grad_local,
        hessian_local,
        m_params.do_newton);
    Eigen::MatrixXd search_dir(1, 2);
    if (!m_params.do_newton) {
        search_dir =
            -Eigen::Map<Eigen::MatrixXd>(grad_local.data(), uv_local.rows(), 2).row(v_map[vid]);
    } else {
        // local hessian for only one node
        Eigen::Matrix2d hessian_at_v;
        hessian_at_v << hessian_local.coeff(v_map[vid], v_map[vid]),
            hessian_local.coeff(v_map[vid], v_map[vid] + vid_onering.size()),
            hessian_local.coeff(v_map[vid] + vid_onering.size(), v_map[vid]),
            hessian_local.coeff(v_map[vid] + vid_onering.size(), v_map[vid] + vid_onering.size());
        Eigen::Vector2d grad_at_v;
        grad_at_v << grad_local(v_map[vid]), grad_local(v_map[vid] + vid_onering.size());
        Eigen::Vector2d newton_at_v = hessian_at_v.ldlt().solve(-grad_at_v);
        search_dir << newton_at_v(0), newton_at_v(1);
    }
    // std::cout << "search_dir" << search_dir << std::endl;
    // do linesearch
    // std::cout << "local E0 = " << local_energy_0 << std::endl;
    auto pos_copy = vertex_attrs[vid].pos;
    double step = 1.0;
    double new_energy;
    auto new_x = uv_local;
    bool ls_good = false;
    for (int i = 0; i < m_params.ls_iters; i++) {
        new_x.row(v_map[vid]) = uv_local.row(v_map[vid]) + step * search_dir;
        vertex_attrs[vid].pos << new_x(v_map[vid], 0), new_x(v_map[vid], 1);
        new_energy = compute_energy(new_x);
        // std::cout << "new E " << new_energy << std::endl;

        bool has_flip = false;
        for (auto loc : locs) {
            if (is_inverted(loc)) {
                has_flip = true;
                break;
            }
        }
        if (new_energy < local_energy_0 && !has_flip) {
            ls_good = true;
            break;
        }
        step = step * 0.8;
    }
    if (ls_good) {
        wmtk::logger()
            .trace("ls good, step = {}, energy {} -> {}", step, local_energy_0, new_energy);
    } else {
        wmtk::logger().trace("ls failed");
        vertex_attrs[vid].pos = pos_copy;
    }


    return ls_good;
}

void extremeopt::ExtremeOpt::smooth_global(int steps)
{
    Eigen::MatrixXi F;
    Eigen::MatrixXd V, uv;
    export_mesh(V, F, uv);
    Eigen::MatrixXi EE;
    export_EE(EE);

    Eigen::VectorXd area;
    Eigen::SparseMatrix<double> G;
    igl::doublearea(V, F, area);
    get_grad_op(V, F, G);
    Eigen::SparseMatrix<double> Aeq;
    buildAeq(EE, uv, F, Aeq);
    Eigen::SparseMatrix<double> AeqT = Aeq.transpose();

    auto compute_energy = [G, area](Eigen::MatrixXd aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G, aaa, Ji);
        return wmtk::compute_energy_from_jacobian(Ji, area);
    };

    // get grad and hessian
    Eigen::SparseMatrix<double> hessian;
    Eigen::VectorXd grad;
    double energy_0 = wmtk::get_grad_and_hessian(G, area, uv, grad, hessian, m_params.do_newton);

    // build kkt system
    Eigen::SparseMatrix<double> kkt(hessian.rows() + Aeq.rows(), hessian.cols() + Aeq.rows());
    buildkkt(hessian, Aeq, AeqT, kkt);
    Eigen::VectorXd rhs(kkt.rows());
    rhs.setZero();
    rhs.topRows(grad.rows()) << -grad;

    // solve the system
    Eigen::SimplicialLDLT<Eigen::SparseMatrix<double>> solver;
    solver.analyzePattern(kkt);
    solver.factorize(kkt);
    Eigen::VectorXd newton = solver.solve(rhs);
    if (solver.info() != Eigen::Success) {
        std::cout << "cannot solve newton system" << std::endl;
        hessian.setIdentity();
        buildkkt(hessian, Aeq, AeqT, kkt);
        solver.analyzePattern(kkt);
        solver.factorize(kkt);
        newton = solver.solve(rhs);
    }

    Eigen::MatrixXd search_dir = Eigen::Map<Eigen::MatrixXd>(newton.data(), V.rows(), 2);

    auto new_x = uv;
    double ls_step_size = 1.0;
    bool ls_good = false;
    for (int i = 0; i < m_params.ls_iters; i++) {
        new_x = uv + ls_step_size * search_dir;
        double new_E = compute_energy(new_x);
        if (new_E < energy_0 && check_flip(new_x, F) == 0) {
            ls_good = true;
            break;
        }
        ls_step_size *= 0.8;
    }

    if (ls_good) {
        // update vertex_attrs
        std::cout << "ls_step_size = " << ls_step_size << std::endl;
        for (int i = 0; i < new_x.rows(); i++) {
            vertex_attrs[i].pos = new_x.row(i);
        }
    } else {
        std::cout << "smooth failed" << std::endl;
    }
}

// TODO: seperate smooth operations and swap operations as 2 functions
void extremeopt::ExtremeOpt::smooth_all_vertices()
{
    auto collect_all_ops = std::vector<std::pair<std::string, Tuple>>();
    for (auto& loc : get_vertices()) {
        collect_all_ops.emplace_back("vertex_smooth", loc);
    }

    auto executor = wmtk::ExecutePass<ExtremeOpt, wmtk::ExecutionPolicy::kSeq>();
    executor(*this, collect_all_ops);
}

void extremeopt::ExtremeOpt::swap_all_edges()
{
    auto collect_all_ops_swap = std::vector<std::pair<std::string, Tuple>>();
    for (auto& loc : get_edges()) {
        collect_all_ops_swap.emplace_back("edge_swap", loc);
    }

    auto setup_and_execute = [&](auto& executor_swap) {
        executor_swap.renew_neighbor_tuples = renew;
        executor_swap.priority = [&](auto& m, auto _, auto& e) {
            return -(vertex_attrs[e.vid(*this)].pos -
                     vertex_attrs[e.switch_vertex(*this).vid(*this)].pos)
                        .norm();
        };
        executor_swap.num_threads = NUM_THREADS;
        executor_swap(*this, collect_all_ops_swap);
    };
    auto executor_swap = wmtk::ExecutePass<ExtremeOpt, wmtk::ExecutionPolicy::kSeq>();
    setup_and_execute(executor_swap);
}

void extremeopt::ExtremeOpt::collapse_all_edges()
{
    auto collect_all_ops_collapse = std::vector<std::pair<std::string, Tuple>>();
    for (auto& loc : get_edges()) {
        if (is_boundary_edge(loc)) {
            collect_all_ops_collapse.emplace_back("test_op", loc);
        } else {
            collect_all_ops_collapse.emplace_back("edge_collapse", loc);
        }
        // collect_all_ops_collapse.emplace_back("test_op", loc);
    }
    auto setup_and_execute = [&](auto& executor_collapse) {
        executor_collapse.renew_neighbor_tuples = renew_collapse;
        // add term with energy
        executor_collapse.priority = [&](auto& m, auto _, auto& e) {
            return (vertex_attrs[e.vid(*this)].pos -
                    vertex_attrs[e.switch_vertex(*this).vid(*this)].pos)
                .norm();
        };
        executor_collapse.num_threads = NUM_THREADS;
        executor_collapse(*this, collect_all_ops_collapse);
        // TODO: priority queue (edge length)
    };
    std::map<Op, std::function<std::optional<std::vector<Tuple>>(ExtremeOpt&, const Tuple&)>>
        test_op = {
            {"test_op", [](ExtremeOpt& m, const Tuple& t) -> std::optional<std::vector<Tuple>> {
                 std::vector<Tuple> ret;

                 ExtremeOpt::CollapsePair ce_op;
                 if (auto [new_t, succ] = ce_op.execute(t, m, ret); succ)
                 {
                    return ret;
                 }
                 return {};
             }}};
    auto executor_collapse = wmtk::ExecutePass<ExtremeOpt, wmtk::ExecutionPolicy::kSeq>(test_op);
    setup_and_execute(executor_collapse);
}


void extremeopt::ExtremeOpt::do_optimization(json& opt_log)
{
    igl::Timer timer;
    double time;

    // prepare to compute energy
    Eigen::SparseMatrix<double> G_global;
    Eigen::MatrixXd V, uv;
    Eigen::MatrixXi F;

    std::cout << "before export" << std::endl;
    export_mesh(V, F, uv);

    get_grad_op(V, F, G_global);
    Eigen::VectorXd dblarea;
    igl::doublearea(V, F, dblarea);
    auto compute_energy = [&G_global, &dblarea](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_global, aaa, Ji);

        return wmtk::compute_energy_from_jacobian(Ji, dblarea);
    };
    auto compute_energy_max = [&G_global, &dblarea, &F](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_global, aaa, Ji);
        auto EVec = wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3));

        // for(int j = 0; j < EVec.size(); ++j) {
        //     if(!std::isfinite(EVec(j))) {
        //         spdlog::info("triangle {} was not finite area {}", j, dblarea(j));
        //         auto f = F.row(j);
        //         for(int j = 0; j < 3; ++j) {
        //             std::cout << aaa.row(f(j)) << "====";
        //         }
        //         std::cout << std::endl;

        //    }
        //}
        return EVec.maxCoeff();
    };

    double E = compute_energy(uv);
    wmtk::logger().info("Start Energy E = {}", E);

    double E_old = E;
    for (int i = 1; i <= m_params.max_iters; i++) {
        double E_max;

        if (this->m_params.do_split)
        {
            split_all_edges();
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            wmtk::logger()
                .info("Mesh F size {}, V size {}, uv size {}", F.rows(), V.rows(), uv.rows());
            wmtk::logger().info("After splitting, E = {}", E);
            wmtk::logger().info("E_max = {}", compute_energy_max(uv));
        }

        if (this->m_params.local_smooth)
        {
            smooth_all_vertices();
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("After LOCAL smoothing {}, E = {}", i, E);
            wmtk::logger().info("E_max = {}", E_max);
        }
        
        if (this->m_params.do_swap) {
            timer.start();
            swap_all_edges();
            time = timer.getElapsedTime();
            wmtk::logger().info("edges swapping operation time serial: {}s", time);
            export_mesh(V, F, uv);

            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            Eigen::VectorXd dblarea2d;
            igl::doublearea(uv, F, dblarea2d);

            std::cout << "min area 3d: " << dblarea.minCoeff() << std::endl;
            std::cout << "min area 2d: " << dblarea2d.minCoeff() << std::endl;

            E = compute_energy(uv);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("After swapping, E = {}", E);
            wmtk::logger().info("E_max = {}", E_max);
        }

        if (this->m_params.do_collapse) {
            collapse_all_edges();
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            wmtk::logger()
                .info("Mesh F size {}, V size {}, uv size {}", F.rows(), V.rows(), uv.rows());
            wmtk::logger().info("After collapsing, E = {}", E);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("E_max = {}", E_max);
        }

        if (this->m_params.global_smooth)
        {
timer.start();
            smooth_global(1);
time = timer.getElapsedTime();
            wmtk::logger().info("GLOBAL smoothing operation time serial: {}s", time);
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("After GLOBAL smoothing {}, E = {}", i, E);
            wmtk::logger().info("E_max = {}", E_max);
        
            opt_log["opt_log"].push_back({{"F_size", F.rows()},{"V_size", V.rows()}, {"E_max", E_max}, {"E_avg", E}});
        }

        // terminate criteria
        // if (E < m_params.E_target) {
        //     wmtk::logger().info(
        //         "Reach target energy({}), optimization succeed!",
        //         m_params.E_target);
        //     break;
        // }
        // if (E == E_old) {
        //     wmtk::logger().info("Energy get stuck, optimization failed.");
        //     break;
        // }

        E_old = E;
        std::cout << std::endl;
    }
}
