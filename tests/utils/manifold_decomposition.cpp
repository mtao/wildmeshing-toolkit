
#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>


TEST_CASE("manifold_decomposition", "[simplex][indexing][dart]")
{
    {
        wmtk::RowVectors2l S(4, 2);
        S.row(0) << 0, 1;
        S.row(1) << 1, 2;
        S.row(2) << 2, 3;
        S.row(3) << 2, 4;
        auto MD = wmtk::utils::internal::boundary_manifold_decomposition<2>(S);

        auto& md = MD.manifold_decomposition;
        REQUIRE(md.rows() == S.rows());
        REQUIRE(md.cols() == S.cols());

        for (const auto& [k, v] : MD.face_map) {
            std::vector<std::string> ds;
            std::transform(v.begin(),v.end(),std::back_inserter(ds), [](const auto& s) -> std::string { return std::string(s); });
            spdlog::info("{} {}", fmt::join(k, ","), fmt::join(ds,","));
        }
        REQUIRE(MD.face_map.size() == 1);
        CHECK(MD.face_map.at(std::array<int64_t, 1>{{2}}).size() == 3);


        auto fm = MD.face_matrix();
        std::cout << MD.manifold_decomposition << std::endl;
        std::cout << fm << std::endl;
        CHECK(fm.rows() == 1);
        REQUIRE(fm.cols() == 1);
        CHECK(fm(0, 0) == 2);
    }

    {
        wmtk::RowVectors3l S(5, 3);
        S.row(0) << 0, 1, 2;
        S.row(1) << 1, 2, 3;
        S.row(2) << 4, 2, 3;
        S.row(3) << 4, 2, 5;
        S.row(4) << 4, 2, 6;
        auto MD = wmtk::utils::internal::boundary_manifold_decomposition<3>(S);

        REQUIRE(MD.face_map.size() == 1);
        CHECK(MD.face_map.at(std::array<int64_t, 2>{{2,4}}).size() == 3);
        std::cout << MD.manifold_decomposition << std::endl;
        std::cout << MD.face_matrix() << std::endl;
    }


    {
        wmtk::RowVectors3l S(9, 3);
        S.row(0) << 0, 1, 4;
        S.row(1) << 0, 1, 5;
        S.row(2) << 0, 1, 6;
        S.row(3) << 0, 2, 7;
        S.row(4) << 0, 2, 8;
        S.row(5) << 0, 2, 9;
        S.row(6) << 0, 3, 10;
        S.row(7) << 0, 3, 11;
        S.row(8) << 0, 3, 12;
        auto MD = wmtk::utils::internal::boundary_manifold_decomposition<3>(S);

        REQUIRE(MD.face_map.size() == 3);
        CHECK(MD.face_map.at(std::array<int64_t, 2>{{0,1}}).size() == 3);
        CHECK(MD.face_map.at(std::array<int64_t, 2>{{0,2}}).size() == 3);
        CHECK(MD.face_map.at(std::array<int64_t, 2>{{0,3}}).size() == 3);
        std::cout << MD.manifold_decomposition << std::endl;
        std::cout << MD.face_matrix() << std::endl;

        auto MD2 = wmtk::utils::internal::boundary_manifold_decomposition<2>(MD.face_matrix());

        REQUIRE(MD2.face_map.size() == 1);
        CHECK(MD2.face_map.at(std::array<int64_t, 1>{{0}}).size() == 3);
        std::cout << MD2.manifold_decomposition << std::endl;
        std::cout << MD2.face_matrix() << std::endl;
    }
}
