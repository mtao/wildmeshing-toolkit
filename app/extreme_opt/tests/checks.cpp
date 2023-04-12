#include "checks.h"
#include <igl/predicates/predicates.h>
#include <catch2/catch.hpp>
#include "ExtremeOpt.h"
#include "SYMDIR.h"
namespace extremeopt {
namespace tests {

double compute_energy(const ExtremeOpt& m)
{
    Eigen::MatrixXd V, uv;
    Eigen::MatrixXi F;
    m.export_mesh(V, F, uv);

    Eigen::SparseMatrix<double> G_global;
    extremeopt::get_grad_op(V, F, G_global);
    Eigen::VectorXd dblarea;
    igl::doublearea(V, F, dblarea);
    Eigen::MatrixXd Ji;
    wmtk::jacobian_from_uv(G_global, uv, Ji);
    return wmtk::compute_energy_from_jacobian(Ji, dblarea) * dblarea.sum();
}
void check_flips(const ExtremeOpt& opt)
{
    check_flips(opt, opt.get_faces());
}
void check_flips(const ExtremeOpt& opt, const wmtk::TriMeshOperation::ExecuteReturnData& retdata)
{
    check_flips(opt, retdata.new_tris);
}
void check_flips(const ExtremeOpt& opt, const std::vector<wmtk::TriMeshTuple>& tuples)
{
    {
        for (const wmtk::TriMeshTuple& face : tuples) {
            const auto& vids = opt.oriented_tri_vids(face);
            const auto& a_db = opt.vertex_attrs.at(vids[0]).pos;
            const auto& b_db = opt.vertex_attrs.at(vids[1]).pos;
            const auto& c_db = opt.vertex_attrs.at(vids[2]).pos;
            CHECK(
                igl::predicates::orient2d(a_db, b_db, c_db) !=
                igl::predicates::Orientation::POSITIVE);
        }
    }
}

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
} // namespace tests
} // namespace extremeopt
