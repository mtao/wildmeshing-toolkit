#include "ExtremeOpt.h"
#include "SYMDIR.h"


#include <wmtk/TriMeshOperation.h>
#include <wmtk/ExecutionScheduler.hpp>

namespace {
auto renew_collapse = [](auto& m, auto op, auto& tris) {
    auto edges = m.new_edges_after(tris);
    auto optup = std::vector<std::pair<std::string, wmtk::TriMesh::Tuple>>();
    for (auto& e : edges) {
        if (m.m_params.with_cons) {
            if (m.is_boundary_edge(e)) {
                optup.emplace_back("test_op", e);
            } else {
                optup.emplace_back("edge_collapse", e);
            }
        } else {
            optup.emplace_back("edge_collapse", e);
        }
    }
    return optup;
};
using namespace extremeopt;
using namespace wmtk;
class CollapsePairOperation : public wmtk::TriMeshOperationShim<ExtremeOpt, CollapsePairOperation>
{
public:
    bool before(ExtremeOpt& m, const TriMesh::Tuple& t)
    {
        // TODO: Relocate this code in before check
        if (!m.is_boundary_edge(t)) {
            // std::cout << "not boundary edge" << std::endl;
            return false;
        }
        if (!t.is_valid(m)) {
            std::cout << "not valid" << std::endl;
            return false;
        }
        if (!m.collapse_edge_before(t)) {
            // std::cout << "link condition error" << std::endl;
            return false;
        }
        Tuple& t_pair_input = t_pair_input_per_thread.local();
        t_pair_input = m.edge_attrs[t.eid(m)].pair;
        if (!m.collapse_edge_before(t_pair_input)) {
            // std::cout << "link condition error" << std::endl;
            return false;
        }
        // Skip cases that paired edges are in the same triangle
        if (t_pair_input.fid(m) == t.fid(m)) {
            return false;
        }
        if (t_pair_input.vid(m) == t.switch_vertex(m).vid(m)) {
            return false;
        }
        if (t_pair_input.switch_vertex(m).vid(m) == t.vid(m)) {
            return false;
        }

        if (m.m_params.use_max_energy) {
            // Get E before collapse
            double E_t_input =
                std::max(m.get_e_max_onering(t), m.get_e_max_onering(t.switch_vertex(m)));
            double E_t_pair_input = std::max(
                m.get_e_max_onering(t_pair_input),
                m.get_e_max_onering(t_pair_input.switch_vertex(m)));
            double E_input = std::max(E_t_input, E_t_pair_input);
            initial_energy_per_thread.local() = E_input;
        } else {
            // Compute Energy before collapse
            double E_t_input = m.get_e_onering_edge(t);
            double E_t_pair_input = m.get_e_max_onering(t_pair_input);
            double E_input = E_t_input + E_t_pair_input;
            initial_energy_per_thread.local() = E_t_input;
        }
        return true;
    }

    bool after(ExtremeOpt& m, ExecuteReturnData& ret_data)
    {
        const TriMesh::Tuple& new_t = ret_data.tuple;
        const TriMesh::Tuple& t = ret_data.tuple;
        Tuple& t_pair_input = t_pair_input_per_thread.local();
        // std::cout << "trying to collapse a boudnary edge" << std::endl;
        // t.print_info();
        // t_pair_input.print_info();
        // std::cout << "E before collapsing is " << E_input << std::endl;

        // get neighbor edges
        auto onering_t_l = m.get_one_ring_edges_for_vertex(t);
        auto onering_t_r = m.get_one_ring_edges_for_vertex(t.switch_vertex(m));
        Tuple bd_t_l, bd_t_r;
        for (auto t_tmp : onering_t_l) {
            if (m.is_boundary_edge(t_tmp)) {
                if (t_tmp.eid(m) != t.eid(m)) {
                    bd_t_l = t_tmp.is_ccw(m) ? t_tmp : t_tmp.switch_vertex(m);
                }
            }
        }
        for (auto t_tmp : onering_t_r) {
            if (m.is_boundary_edge(t_tmp)) {
                if (t_tmp.eid(m) != t.eid(m)) {
                    bd_t_r = t_tmp.is_ccw(m) ? t_tmp : t_tmp.switch_vertex(m);
                }
            }
        }

        Tuple bd_t_l_pair = m.edge_attrs[bd_t_l.eid(m)].pair;
        Tuple bd_t_r_pair = m.edge_attrs[bd_t_r.eid(m)].pair;
        bool keep_t = false, keep_t_opp = false;
        if (bd_t_r_pair.switch_vertex(m).vid(m) == t_pair_input.vid(m)) {
            auto len1 =
                (m.vertex_attrs[t.vid(m)].pos - m.vertex_attrs[bd_t_r.switch_vertex(m).vid(m)].pos)
                    .norm();
            auto len2 = (m.vertex_attrs[t_pair_input.switch_vertex(m).vid(m)].pos -
                         m.vertex_attrs[bd_t_r_pair.vid(m)].pos)
                            .norm();
            if (std::abs(len1 - len2) < 1e-7) {
                // std::cout << "keep t.vid" << std::endl;
                keep_t = true;
            } else {
                // std::cout << "len diff, cannot keep t.vid" << std::endl;
            }
        } else {
            // std::cout << "cannot keep t.vid" << std::endl;
        }

        if (bd_t_l_pair.vid(m) == t_pair_input.switch_vertex(m).vid(m)) {
            auto len1 =
                (m.vertex_attrs[t.switch_vertex(m).vid(m)].pos - m.vertex_attrs[bd_t_l.vid(m)].pos)
                    .norm();
            auto len2 = (m.vertex_attrs[t_pair_input.vid(m)].pos -
                         m.vertex_attrs[bd_t_l_pair.switch_vertex(m).vid(m)].pos)
                            .norm();
            if (std::abs(len1 - len2) < 1e-7) {
                // std::cout << "keep t.switch_vertex.vid" << std::endl;
                keep_t_opp = true;
            } else {
                // std::cout << "len diff, cannot keep t.switch_vertex.vid" << std::endl;
            }
        } else {
            // std::cout << "cannot keep t.switch_vertex.vid" << std::endl;
        }
        if (!keep_t && !keep_t_opp) {
            // std::cout << "this boudnary edge cannot collapse" << std::endl;
            ret_data.success = false;
            return ret_data;
        }
        Eigen::Vector3d V_keep_t, V_keep_t_pair;
        Eigen::Vector2d uv_keep_t, uv_keep_t_pair;
        if (keep_t) {
            V_keep_t = m.vertex_attrs[t.vid(m)].pos_3d;
            uv_keep_t = m.vertex_attrs[t.vid(m)].pos;
            V_keep_t_pair = m.vertex_attrs[t_pair_input.switch_vertex(m).vid(m)].pos_3d;
            uv_keep_t_pair = m.vertex_attrs[t_pair_input.switch_vertex(m).vid(m)].pos;
        } else {
            V_keep_t = m.vertex_attrs[t.switch_vertex(m).vid(m)].pos_3d;
            uv_keep_t = m.vertex_attrs[t.switch_vertex(m).vid(m)].pos;
            V_keep_t_pair = m.vertex_attrs[t_pair_input.vid(m)].pos_3d;
            uv_keep_t_pair = m.vertex_attrs[t_pair_input.vid(m)].pos;
        }

        // new_t = m.collapse_edge_new(t, new_tris);


        double E_t, E_t_pair;
        if (!m.collapse_bd_edge_after(new_t, V_keep_t, uv_keep_t, bd_t_l, bd_t_r, E_t)) {
            // std::cout << "collapse t fail" << std::endl;
            ret_data.success = false;
            ;
            return ret_data;
        } else {
            // std::cout << "collapse t ok" << std::endl;
        }

        Tuple t_pair =
            m.tuple_from_edge(t_pair_input.eid_unsafe(m) / 3, t_pair_input.eid_unsafe(m) % 3);
        auto onering_t_pair_l = m.get_one_ring_edges_for_vertex(t_pair);
        auto onering_t_pair_r = m.get_one_ring_edges_for_vertex(t_pair.switch_vertex(m));
        Tuple bd_t_pair_l, bd_t_pair_r;
        for (auto t_tmp : onering_t_pair_l) {
            if (m.is_boundary_edge(t_tmp)) {
                if (t_tmp.eid(m) != t_pair.eid(m)) {
                    bd_t_pair_l = t_tmp.is_ccw(m) ? t_tmp : t_tmp.switch_vertex(m);
                }
            }
        }
        for (auto t_tmp : onering_t_pair_r) {
            if (m.is_boundary_edge(t_tmp)) {
                if (t_tmp.eid(m) != t_pair.eid(m)) {
                    bd_t_pair_r = t_tmp.is_ccw(m) ? t_tmp : t_tmp.switch_vertex(m);
                }
            }
        }
        // new_t = m.collapse_edge_new(t_pair, new_tris);
        if (!m.collapse_bd_edge_after(
                new_t,
                V_keep_t_pair,
                uv_keep_t_pair,
                bd_t_pair_l,
                bd_t_pair_r,
                E_t_pair)) {
            // std::cout << "collapse t pair fail" << std::endl;
            ret_data.success = false;
            ;
            return ret_data;
        } else {
            // std::cout << "collapse t pair ok" << std::endl;
        }
        double current_energy;

        if (m.m_params.use_max_energy) {
            current_energy = std::max(E_t, E_t_pair);
        } else {
            current_energy = E_t + E_t_pair;
            ;
        }
        if (initial_energy_per_thread.local() < current_energy) {
            ret_data.success = false;
        }
        return ret_data;
    }

    ExecuteReturnData execute(ExtremeOpt& m, const Tuple& t)
    {
        ExecuteReturnData ret_data;
        std::vector<Tuple>& new_tris = ret_data.new_tris;
        Tuple& new_t = ret_data.tuple;

        const Tuple& t_pair_input = t_pair_input_per_thread.local();
        Tuple t_pair =
            m.tuple_from_edge(t_pair_input.eid_unsafe(m) / 3, t_pair_input.eid_unsafe(m) % 3);

        ret_data.combine(m_edge_collapser.execute(m, t));
        ret_data.combine(m_edge_collapser.execute(m, t_pair));
        return ret_data;
    }

    std::string name() const override { return "collapse_pair"; }
    CollapsePairOperation(){};
    virtual ~CollapsePairOperation(){};

    wmtk::TriMeshEdgeCollapseOperation m_edge_collapser;
    tbb::enumerable_thread_specific<Tuple> t_pair_input_per_thread;
    tbb::enumerable_thread_specific<double> initial_energy_per_thread;
};


class ExtremeOptEdgeCollapseOperation : public wmtk::TriMeshOperationShim<
                                            ExtremeOpt,
                                            ExtremeOptEdgeCollapseOperation,
                                            wmtk::TriMeshEdgeCollapseOperation>
{
public:
    ExecuteReturnData execute(ExtremeOpt& m, const Tuple& t)
    {
        return wmtk::TriMeshEdgeCollapseOperation::execute(m, t);
    }
    bool before(ExtremeOpt& m, const Tuple& t)
    {
        if (wmtk::TriMeshEdgeCollapseOperation::before(m, t)) {
            return m.collapse_edge_before(t);
        }
        return false;
    }
    bool after(ExtremeOpt& m, ExecuteReturnData& ret_data)
    {
        if (wmtk::TriMeshEdgeCollapseOperation::after(m, ret_data)) {
            ret_data.success |= m.collapse_edge_after(ret_data.tuple);
        }
        return ret_data;
    }
    bool invariants(ExtremeOpt& m, ExecuteReturnData& ret_data)
    {
        if (wmtk::TriMeshEdgeCollapseOperation::invariants(m, ret_data)) {
            ret_data.success |= m.invariants(ret_data.new_tris);
        }
        return ret_data;
    }
};

template <typename Executor>
void addCustomOps(Executor& e)
{
    e.add_operation(std::make_shared<ExtremeOptEdgeCollapseOperation>());
    e.add_operation(std::make_shared<CollapsePairOperation>());
}
} // namespace
bool extremeopt::ExtremeOpt::collapse_edge_before(const Tuple& t)
{
    if (!t.is_valid(*this)) {
        return false;
    }

    // avoid boundary edges here
    if (m_params.with_cons && is_boundary_edge(t)) {
        std::cout << "Error: Wrong function call when collapsing a boudnary edge" << std::endl;
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
    auto& v = vertex_attrs[vid];
    v.pos_3d = V;
    v.pos = uv;

    // get local F,V,uv
    auto vid_onering = get_one_ring_vids_for_vertex(vid);

    // check edgelen
    for (int vid_tmp : vid_onering) {
        auto V_tmp = vertex_attrs[vid_tmp].pos_3d;
        auto uv_tmp = vertex_attrs[vid_tmp].pos;
        double elen_3d = (V_tmp - V).norm();
        double elen = (uv_tmp - uv).norm();
        if (elen > elen_threshold) return false;
        if (elen_3d > elen_threshold_3d) return false;
    }

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
    Eigen::MatrixXd Ji;
    wmtk::jacobian_from_uv(G_local, uv_local, Ji);
    double E_after_collapse =
        wmtk::compute_energy_from_jacobian(Ji, area_local_3d) * area_local_3d.sum();

    std::cout << "E_after collapse: " << E_after_collapse << " area: " << area_local_3d.sum()
              << std::endl;
    // std::cout << "E_after collapse: " << E_after_collapse  << " area: " << area_local_3d.sum() << std::endl;
    if (E_after_collapse > position_cache.local().E_before_collapse) {
        // E_sum does not go down
        return false;
    }
    // std::cout << "collapse good" << std::endl;

    if (m_params.with_cons) {
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
    }

    return true;
}

bool extremeopt::ExtremeOpt::collapse_bd_edge_after(
    const Tuple& t,
    const Eigen::Vector3d& V_keep,
    const Eigen::Vector2d& uv_keep,
    Tuple& t_l_old,
    Tuple& t_r_old,
    double& E)
{
    // update vertex position
    auto vid = t.vid(*this);
    vertex_attrs[vid].pos_3d = V_keep;
    vertex_attrs[vid].pos = uv_keep;

    // get local mesh and check area/E
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
    auto Es = wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3));
    for (int i = 0; i < Es.size(); i++) {
        if (!std::isfinite(Es(i))) {
            return false;
        }
    }

    // compute energy here
    E = Es.dot(area_local_3d);

    // check envelope
    if (!invariants(get_one_ring_tris_for_vertex(t))) {
        return false;
    }
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

void extremeopt::ExtremeOpt::collapse_all_edges()
{
    auto collect_all_ops_collapse = std::vector<std::pair<std::string, Tuple>>();
    for (auto& loc : get_edges()) {
        if (m_params.with_cons) {
            if (is_boundary_edge(loc)) {
                collect_all_ops_collapse.emplace_back("collapse_pair", loc);
            } else {
                collect_all_ops_collapse.emplace_back(TriMeshEdgeCollapseOperation{}.name(), loc);
            }
        } else {
            collect_all_ops_collapse.emplace_back("edge_collapse", loc);
        }
        // collect_all_ops_collapse.emplace_back("collapse_pair", loc);
    }
    auto setup_and_execute = [&](auto& executor_collapse) {
        addCustomOps(executor_collapse);
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
    auto executor_collapse = wmtk::ExecutePass<ExtremeOpt, wmtk::ExecutionPolicy::kSeq>();
    setup_and_execute(executor_collapse);
}
