//
// Created by Yixin Hu on 11/3/21.
//

#include "TetWild.h"

#include <MshSaver.h>
#include <Logger.hpp>

#include <wmtk/AMIPS.h>
#include <wmtk/TetraQualityUtils.hpp>

#include <igl/predicates/predicates.h>
#include <spdlog/fmt/ostr.h>

bool tetwild::TetWild::is_inverted(const Tuple& loc)
{
    std::array<Vector3f, 4> ps;
    auto its = loc.oriented_tet_vertices(*this);
    for (int j = 0; j < 4; j++) {
        ps[j] = m_vertex_attribute[its[j].vid()].m_posf;
    }

    //
    igl::predicates::exactinit();
    auto res = igl::predicates::orient3d(ps[0], ps[1], ps[2], ps[3]);
    Scalar result;
    if (res == igl::predicates::Orientation::POSITIVE)
        result = 1;
    else if (res == igl::predicates::Orientation::NEGATIVE)
        result = -1;
    else
        result = 0;

    if (result <= 0) return true;
    return false;
}

double tetwild::TetWild::get_quality(const Tuple& loc)
{
    std::array<Vector3f, 4> ps;
    auto its = loc.oriented_tet_vertices(*this);
    for (int j = 0; j < 4; j++) {
        ps[j] = m_vertex_attribute[its[j].vid()].m_posf;
    }

    std::array<double, 12> T;
    for (int j = 0; j < 3; j++) {
        T[0 * 3 + j] = ps[0][j];
        T[1 * 3 + j] = ps[1][j];
        T[2 * 3 + j] = ps[2][j];
        T[3 * 3 + j] = ps[3][j];
    }

    double energy = wmtk::AMIPS_energy(T);
    if (std::isinf(energy) || std::isnan(energy) || energy < 3 - 1e-3) return MAX_ENERGY;
    return energy;
}


bool tetwild::TetWild::vertex_invariant(const Tuple& t)
{
    int v_id = t.vid();

    // check rounded

    // check surface
    if (m_vertex_attribute[v_id].m_is_on_surface) {
    }

    return true;
}

bool tetwild::TetWild::tetrahedron_invariant(const Tuple& t)
{
    // check inversion

    return true;
}

void tetwild::TetWild::smooth_all_vertices()
{
    auto tuples = get_vertices();
    apps::logger().debug("tuples");
    auto cnt_suc = 0;
    for (auto& t : tuples) { // TODO: threads
        if (smooth_vertex(t)) cnt_suc++;
    }
    apps::logger().debug("Smoothing Success Count {}", cnt_suc);
}

bool tetwild::TetWild::smooth_before(const Tuple& t)
{
    return true;
}

bool tetwild::TetWild::smooth_after(const Tuple& t)
{
    // Newton iterations are encapsulated here.
    // TODO: bbox/surface tags.
    // TODO: envelope check.
    apps::logger().trace("Newton iteration for vertex smoothing.");
    auto vid = t.vid();

    auto locs = t.get_conn_tets(*this);
    assert(locs.size() > 0);
    std::vector<std::array<double, 12>> assembles(locs.size());
    auto loc_id = 0;

    for (auto& loc : locs) {
        auto& T = assembles[loc_id];
        auto t_id = loc.tid();

        assert(!is_inverted(loc));
        auto local_tuples = loc.oriented_tet_vertices(*this);
        std::array<size_t, 4> local_verts;
        for (auto i = 0; i < 4; i++) {
            local_verts[i] = local_tuples[i].vid();
        }

        local_verts = wmtk::orient_preserve_tet_reorder(local_verts, vid);

        for (auto i = 0; i < 4; i++) {
            for (auto j = 0; j < 3; j++) {
                T[i * 3 + j] = m_vertex_attribute[local_verts[i]].m_posf[j];
            }
        }
        loc_id++;
    }


    auto old_pos = m_vertex_attribute[vid].m_posf;
    m_vertex_attribute[vid].m_posf = wmtk::newton_direction_from_stack(assembles);
    apps::logger().trace(
        "old pos {} -> new pos {}",
        old_pos.transpose(),
        m_vertex_attribute[vid].m_posf.transpose());
    // note: duplicate code snippets.
    for (auto& loc : locs) {
        if (is_inverted(loc)) {
            m_vertex_attribute[vid].m_posf = old_pos;
            return false;
        }
    }

    for (auto& loc : locs) {
        auto t_id = loc.tid();
        m_tet_attribute[t_id].m_qualities = get_quality(loc);
    }
    return true;
}

void tetwild::TetWild::output_mesh(std::string file)
{
    PyMesh::MshSaver mSaver(file, true);

    Eigen::VectorXd V_flat(3 * m_vertex_attribute.size());
    for (int i = 0; i < m_vertex_attribute.size(); i++) {
        for (int j = 0; j < 3; j++) V_flat(3 * i + j) = m_vertex_attribute[i].m_posf[j];
    }

    Eigen::VectorXi T_flat(4 * n_tets());
    for (int i = 0; i < n_tets(); i++) {
        Tuple loc = tuple_from_tet(i);
        auto vs = oriented_tet_vertices(loc);
        for (int j = 0; j < 4; j++) {
            T_flat(4 * i + j) = vs[j].vid();
        }
    }

    mSaver.save_mesh(V_flat, T_flat, 3, mSaver.TET);
}