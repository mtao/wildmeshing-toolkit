#include <igl/PI.h>
#include <spdlog/common.h>
#include <catch2/catch.hpp>
#include "ExtremeOpt.h"
#include "Models.h"
#include "checks.h"

namespace {


void init_model(extremeopt::ExtremeOpt& extremeopt, const std::string& model)
{
    extremeopt.m_param.model_name = model;
    extremeopt::tests::create(extremeopt, model);
}


extremeopt::Parameters default_params()
{

    extremeopt::Parameters param;
    param.load("path_to_js");
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


void test_collapse_all(const std::string& model_name)
{
    extremeopt::ExtremeOpt extremeopt;
    extremeopt.m_params = default_params();
    extremeopt.m_params.do_collapse = true;
    extremeopt::tests::create(extremeopt, model_name);
    consolidate_mesh_cons(); // use the one with constraints

    double E_in = extremeopt::tests::compute_energy(extremeopt);

    extremeopt.collapse_all_edges();
    consolidate_mesh_cons(); // use the one with constraints

    extremeopt::tests::perform_checks(extremeopt, E_in);
}
void test_swap_all(const std::string& model_name)
{
    extremeopt::ExtremeOpt extremeopt;
    extremeopt.m_params = default_params();
    extremeopt.m_params.do_swap = true;
    extremeopt::tests::create(extremeopt, model_name);

    consolidate_mesh_cons(); // use the one with constraints
    double E_in = extremeopt::tests::compute_energy(extremeopt);

    extremeopt.swap_all_edges();

    consolidate_mesh_cons(); // use the one with constraints
    extremeopt::tests::perform_checks(extremeopt, E_in);
}

void test_smooth_all(const std::string& model_name)
{
    extremeopt::ExtremeOpt extremeopt;
    extremeopt.m_params = default_params();
    extremeopt.m_params.do_smooth = true;
    extremeopt::tests::create(extremeopt, model_name);
    consolidate_mesh_cons(); // use the one with constraints

    double E_in = extremeopt::tests::compute_energy(extremeopt);

    extremeopt.smooth_all_edges();

    consolidate_mesh_cons(); // use the one with constraints
    extremeopt::tests::perform_checks(extremeopt, E_in);
}
void test_model(const std::string& model_name)
{
    test_collapse_all(model_name);
    test_swap_all(model_name);
    test_smooth_all(model_name);
}

TEST_CASE("helmet", "[helmet]")
{
    test_model("helmet");
}

TEST_CASE("eight", "[eight]")
{
    test_model("eight");
}

