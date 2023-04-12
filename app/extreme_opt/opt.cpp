
#include "CollapsePairOperation.h"
#include "ExtremeOpt.h"
#include "wmtk/ExecutionScheduler.hpp"

#include <Eigen/src/Core/util/Constants.h>
#include <igl/Timer.h>

#include <Eigen/Sparse>
#include <array>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/TriQualityUtils.hpp>

#include <igl/upsample.h>
#include <igl/writeOBJ.h>
#include <limits>
#include <optional>
#include <wmtk/utils/TupleUtils.hpp>
#include "SYMDIR.h"

using namespace wmtk;


void extremeopt::ExtremeOpt::do_optimization(nlohmann::json& opt_log)
{
    igl::Timer timer;
    double time;

    // prepare to compute energy
    Eigen::SparseMatrix<double> G_global;
    Eigen::MatrixXd V, uv;
    Eigen::MatrixXi F;

    consolidate_mesh_cons(); // use the one with constraints
    export_mesh(V, F, uv);
    // get edge length thresholds for collapsing operation
    elen_threshold = 0;
    elen_threshold_3d = 0;
    for (int i = 0; i < F.rows(); i++) {
        for (int j = 0; j < 3; j++) {
            int v1 = F(i, j);
            int v2 = F(i, (j + 1) % 3);
            double elen = (uv.row(v1) - uv.row(v2)).norm();
            double elen_3d = (V.row(v1) - V.row(v2)).norm();
            if (elen > elen_threshold) elen_threshold = elen;
            if (elen_3d > elen_threshold_3d) elen_threshold_3d = elen_3d;
        }
    }
    elen_threshold *= m_params.elen_alpha;
    elen_threshold_3d *= m_params.elen_alpha;

    get_grad_op(V, F, G_global);
    Eigen::VectorXd dblarea;
    igl::doublearea(V, F, dblarea);
    auto compute_energy = [&G_global, &dblarea](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_global, aaa, Ji);
        return wmtk::compute_energy_from_jacobian(Ji, dblarea) * dblarea.sum();
    };

    auto compute_energy_avg = [&G_global, &dblarea](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_global, aaa, Ji);
        return wmtk::compute_energy_from_jacobian(Ji, dblarea);
    };

    auto compute_energy_max = [&G_global, &dblarea, &F, &V](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_global, aaa, Ji);
        auto EVec = wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3));
        for (int j = 0; j < EVec.size(); ++j) {
            if (!std::isfinite(EVec(j))) {
                auto f = F.row(j);
                spdlog::info(
                    "triangle {} was not finite area {} ({})",
                    j,
                    dblarea(j),
                    fmt::join(f, ","));
            }
        }

        return EVec.maxCoeff();
    };
    auto compute_energy_all = [&G_global, &dblarea, &F](Eigen::MatrixXd& aaa) {
        Eigen::MatrixXd Ji;
        wmtk::jacobian_from_uv(G_global, aaa, Ji);
        return wmtk::symmetric_dirichlet_energy(Ji.col(0), Ji.col(1), Ji.col(2), Ji.col(3));
    };

    double E = compute_energy(uv);
    wmtk::logger().info("Start Energy E = {}", E);
    wmtk::logger().info("Start E_max = {}", compute_energy_max(uv));
    opt_log["opt_log"].push_back(
        {{"F_size", F.rows()},
         {"V_size", V.rows()},
         {"E_max", compute_energy_max(uv)},
         {"E", E},
         {"E_avg", E / dblarea.sum()}});
    double E_old = E;
    for (int i = 1; i <= m_params.max_iters; i++) {
        double E_max;

        if (this->m_params.do_split) {
            auto Es = compute_energy_all(uv);
            split_all_edges(Es);

            consolidate_mesh_cons(); // use the one with constraints
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            wmtk::logger()
                .info("Mesh F size {}, V size {}, uv size {}", F.rows(), V.rows(), uv.rows());
            wmtk::logger().info("After splitting, E = {}", E);
            wmtk::logger().info("E_avg = {}", E / dblarea.sum());
            wmtk::logger().info("E_max = {}", compute_energy_max(uv));
            spdlog::info("E is {} {} {}", std::isfinite(E), !std::isnan(E), !std::isinf(E));
        }
        if (m_params.save_meshes)
            igl::writeOBJ(
                "new_tests/" + m_params.model_name + "_step_" + std::to_string(i) + "_splitted.obj",
                V,
                F,
                V,
                F,
                uv,
                F);

        if (this->m_params.do_swap) {
            timer.start();
            swap_all_edges();
            time = timer.getElapsedTime();
            wmtk::logger().info("edges swapping operation time serial: {}s", time);
            consolidate_mesh_cons(); // use the one with constraints
            export_mesh(V, F, uv);

            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("After swapping, E = {}", E);
            wmtk::logger().info("E_avg = {}", E / dblarea.sum());
            wmtk::logger().info("E_max = {}", E_max);
        }
        if (m_params.save_meshes)
            igl::writeOBJ(
                "new_tests/" + m_params.model_name + "_step_" + std::to_string(i) + "_swapped.obj",
                V,
                F,
                V,
                F,
                uv,
                F);

        if (this->m_params.do_collapse) {
            collapse_all_edges();
            consolidate_mesh_cons(); // use the one with constraints
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            wmtk::logger()
                .info("Mesh F size {}, V size {}, uv size {}", F.rows(), V.rows(), uv.rows());
            wmtk::logger().info("After collapsing, E = {}", E);
            wmtk::logger().info("E_avg = {}", E / dblarea.sum());
            E_max = compute_energy_max(uv);
            wmtk::logger().info("E_max = {}", E_max);
        }
        if (m_params.save_meshes)
            igl::writeOBJ(
                "new_tests/" + m_params.model_name + "_step_" + std::to_string(i) +
                    "_collapsed.obj",
                V,
                F,
                V,
                F,
                uv,
                F);

        if (this->m_params.local_smooth) {
            timer.start();
            smooth_all_vertices();
            time = timer.getElapsedTime();
            wmtk::logger().info("LOCAL smoothing operation time serial: {}s", time);
            consolidate_mesh_cons(); // use the one with constraints
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("After LOCAL smoothing {}, E = {}", i, E);
            wmtk::logger().info("E_avg = {}", E / dblarea.sum());
            wmtk::logger().info("E_max = {}", E_max);
        }
        if (this->m_params.global_smooth) {
            timer.start();
            smooth_global(1);
            time = timer.getElapsedTime();
            wmtk::logger().info("GLOBAL smoothing operation time serial: {}s", time);
            consolidate_mesh_cons(); // use the one with constraints
            export_mesh(V, F, uv);
            get_grad_op(V, F, G_global);
            igl::doublearea(V, F, dblarea);
            E = compute_energy(uv);
            E_max = compute_energy_max(uv);
            wmtk::logger().info("After GLOBAL smoothing {}, E = {}", i, E);
            wmtk::logger().info("E_avg = {}", E / dblarea.sum());
            wmtk::logger().info("E_max = {}", E_max);
        }
        if (m_params.save_meshes)
            igl::writeOBJ(
                "new_tests/" + m_params.model_name + "_step_" + std::to_string(i) + "_smoothed.obj",
                V,
                F,
                V,
                F,
                uv,
                F);
        opt_log["opt_log"].push_back(
            {{"F_size", F.rows()},
             {"V_size", V.rows()},
             {"E_max", E_max},
             {"E", E},
             {"E_avg", E / dblarea.sum()}});
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
