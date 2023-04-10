#include <igl/PI.h>
#include <igl/predicates/predicates.h>
#include <spdlog/common.h>
#include <catch2/catch.hpp>
#include "ExtremeOpt.h"
#include "Models.h"
#include "SYMDIR.h"

namespace {
void check_flip(const Eigen::MatrixXd& uv, const Eigen::MatrixXi& Fn)
{
    for (int i = 0; i < Fn.rows(); i++) {
        auto f = Fn.row(i);
        auto a_db = uv.row(f(0));
        auto b_db = uv.row(f(1));
        auto c_db = uv.row(f(2));
        CHECK(
            igl::predicates::orient2d(a_db, b_db, c_db) != igl::predicates::Orientation::POSITIVE);
    }
}
void check_flip(const extremeopt::ExtremeOpt& opt)
{
    for (const wmtk::TriMeshTuple& face : opt.get_faces()) {
        const auto& vids = opt.oriented_tri_vids(face);
        const auto& a_db = opt.vertex_attrs.at(vids[0]).pos;
        const auto& b_db = opt.vertex_attrs.at(vids[1]).pos;
        const auto& c_db = opt.vertex_attrs.at(vids[2]).pos;
        CHECK(
            igl::predicates::orient2d(a_db, b_db, c_db) != igl::predicates::Orientation::POSITIVE);
    }
}


void init_model(extremeopt::ExtremeOpt& extremeopt, const std::string& model)
{
    extremeopt.m_param.model_name = model;
    extremeopt::tests::create(extremeopt, model);
}

double compute_energy(const extremeopt::ExtremeOpt& m)
{
    Eigen::MatrixXd V, uv;
    Eigen::MatrixXi F;
    extremeopt.export_mesh(V, F, uv);

    Eigen::SparseMatrix<double> G_global;
    extremeopt::get_grad_op(V, F, G_global);
    Eigen::VectorXd dblarea;
    igl::doublearea(V, F, dblarea);
    Eigen::MatrixXd Ji;
    wmtk::jacobian_from_uv(G_global, uv, Ji);
    return wmtk::compute_energy_from_jacobian(Ji, dblarea) * dblarea.sum();
}


extremeopt::Parameters default_params()
{
    extremeopt::Parameters param;
    param.max_iters = 1;
    param.local_smooth = false;
    param.global_smooth = false;
    param.use_envelope = true;
    param.elen_alpha = 0.2;
    param.do_projection = false;
    param.with_cons = true;
    param.save_meshes = false;
    param.do_collapse = false;
    return params;
}
} // namespace

void perform_checks(const ExtremeOpt& extremeopt, const double E_in)
{
    double E_out = compute_energy(extremeopt);
    SECTION("no flip")
    {
        check_flip(extremeopt);
    }
    SECTION("seamless constraints")
    {
        REQUIRE(extremeopt.check_constraints());
    }
    SECTION("smaller energy")
    {
        REQUIRE(E_out <= E_in);
    }
}

void test_collapse(const std::string& model_name)
{
    extremeopt::ExtremeOpt extremeopt;
    extremeopt.m_params = default_params();
    extremeopt.m_params.do_collapse = true;
    extremeopt::tests::create(extremeopt, model_name);

    double E_in = compute_energy(extremeopt);

    extremeopt.collapse_all_edges();

    perform_checks(extremeopt, E_in);
}
void test_swap(const std::string& model_name)
{
    extremeopt::ExtremeOpt extremeopt;
    extremeopt.m_params = default_params();
    extremeopt.m_params.do_swap = true;
    extremeopt::tests::create(extremeopt, model_name);

    double E_in = compute_energy(extremeopt);

    extremeopt.swap_all_edges();

    perform_checks(extremeopt, E_in);
}

void test_smooth(const std::string& model_name)
{
    extremeopt::ExtremeOpt extremeopt;
    extremeopt.m_params = default_params();
    extremeopt.m_params.do_smooth = true;
    extremeopt::tests::create(extremeopt, model_name);

    double E_in = compute_energy(extremeopt);

    extremeopt.smooth_all_edges();

    perform_checks(extremeopt, E_in);
}
void test_model(const std::string& model_name)
{
    test_collapse(model_name);
    test_swap(model_name);
    test_smooth(model_name);
}

TEST_CASE("helmet", "[helmet]")
{
    test_model("helmet");
}

TEST_CASE("eight", "[eight]")
{
    test_model("eight");
}

