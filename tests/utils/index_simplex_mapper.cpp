#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/internal/IndexSimplexMapper.hpp>
#include "../dart/utils//canonical_darts.hpp"


TEST_CASE("index_simplex_mapper", "[simplex][indexing][dart]")
{
    {
        wmtk::MatrixXl S(3, 2);

        S.row(0) << 0, 1;
        S.row(1) << 2, 3;
        S.row(2) << 1, 2;

        wmtk::utils::internal::IndexSimplexMapper ism(S);


        std::array<int64_t, 2> a{{0, 1}};
        std::array<int64_t, 2> b{{1, 0}};
        std::array<int64_t, 2> c{{2, 3}};
        std::array<int64_t, 2> d{{3, 2}};
        std::array<int64_t, 2> e{{1, 2}};
        std::array<int64_t, 2> f{{2, 1}};
        auto ad = ism.get_dart(a);
        auto bd = ism.get_dart(b);
        auto cd = ism.get_dart(c);
        auto dd = ism.get_dart(d);
        auto ed = ism.get_dart(e);
        auto fd = ism.get_dart(f);

        CHECK((ad == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d01.permutation())));
        CHECK((bd == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d10.permutation())));
        CHECK((cd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d01.permutation())));
        CHECK((dd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d10.permutation())));
        CHECK((ed == wmtk::dart::Dart(2, wmtk::tests::dart::utils::d01.permutation())));
        CHECK((fd == wmtk::dart::Dart(2, wmtk::tests::dart::utils::d10.permutation())));
    }

    {
        wmtk::MatrixXl S(2, 3);

        S.row(0) << 0, 1, 2;
        S.row(1) << 3, 2, 1;

        wmtk::utils::internal::IndexSimplexMapper ism(S);


        {
            std::array<int64_t, 3> a{{0, 1, 2}};
            std::array<int64_t, 3> b{{0, 2, 1}};
            std::array<int64_t, 3> c{{1, 0, 2}};
            std::array<int64_t, 3> d{{1, 2, 0}};
            std::array<int64_t, 3> e{{2, 0, 1}};
            std::array<int64_t, 3> f{{2, 1, 0}};
            auto ad = ism.get_dart(a);
            auto bd = ism.get_dart(b);
            auto cd = ism.get_dart(c);
            auto dd = ism.get_dart(d);
            auto ed = ism.get_dart(e);
            auto fd = ism.get_dart(f);

            CHECK((ad == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d012.permutation())));
            CHECK((bd == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d021.permutation())));
            CHECK((cd == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d102.permutation())));
            CHECK((dd == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d120.permutation())));
            CHECK((ed == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d201.permutation())));
            CHECK((fd == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d210.permutation())));
        }

        {
            std::array<int64_t, 3> a{{3, 2, 1}};
            std::array<int64_t, 3> b{{3, 1, 2}};
            std::array<int64_t, 3> c{{2, 3, 1}};
            std::array<int64_t, 3> d{{2, 1, 3}};
            std::array<int64_t, 3> e{{1, 3, 2}};
            std::array<int64_t, 3> f{{1, 2, 3}};
            auto ad = ism.get_dart(a);
            auto bd = ism.get_dart(b);
            auto cd = ism.get_dart(c);
            auto dd = ism.get_dart(d);
            auto ed = ism.get_dart(e);
            auto fd = ism.get_dart(f);

            CHECK((ad == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d012.permutation())));
            CHECK((bd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d021.permutation())));
            CHECK((cd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d102.permutation())));
            CHECK((dd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d120.permutation())));
            CHECK((ed == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d201.permutation())));
            CHECK((fd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d210.permutation())));
        }
    }
}
