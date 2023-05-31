#include <TetWild.h>

#include <wmtk/TetMesh.h>

#include <catch2/catch.hpp>

#include <wmtk/utils/triangle_mesh_io.h>

using namespace wmtk;
using namespace tetwild;

TEST_CASE("optimize_mesh", "[test_runner]")
{
    const std::string root(WMTK_DATA_DIR);
    const std::string path = root + "/Octocat.obj";
    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    bool ok = wmtk::read_triangle_mesh(path, V, F);

    REQUIRE(ok);
    REQUIRE(V.rows() == 18944);
    REQUIRE(F.rows() == 37884);
}
