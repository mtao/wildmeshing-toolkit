
#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>


TEST_CASE("manifold_decomposition", "[simplex][indexing][dart]")
{

    {
    wmtk::RowVectors2l S(4,2);
    S.row(0) << 0,1;
    S.row(1) << 1,2;
    S.row(2) << 2,3;
    S.row(3) << 2,4;
    auto [S2,B] = wmtk::utils::internal::boundary_manifold_decomposition<2>(S);

    std::cout << S2 << std::endl;
    std::cout << B << std::endl;

    }

    {
    wmtk::RowVectors3l S(5,3);
    S.row(0) << 0,1,2;
    S.row(1) << 1,2,3;
    S.row(2) << 4,2,3;
    S.row(3) << 4,2,5;
    S.row(4) << 4,2,6;
    auto [S2,B] = wmtk::utils::internal::boundary_manifold_decomposition<3>(S);

    std::cout << S2 << std::endl;
    std::cout << B << std::endl;
    }




}
