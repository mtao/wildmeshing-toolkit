#include <Eigen/Dense>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/internal/IndexSimplexMapper.hpp>
#include "../dart/utils//canonical_darts.hpp"
#include <wmtk/TriMesh.hpp>
#include <wmtk/EdgeMesh.hpp>


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
        auto ad = ism.get_internal_dart(a);
        auto bd = ism.get_internal_dart(b);
        auto cd = ism.get_internal_dart(c);
        auto dd = ism.get_internal_dart(d);
        auto ed = ism.get_internal_dart(e);
        auto fd = ism.get_internal_dart(f);

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
            auto ad = ism.get_internal_dart(a);
            auto bd = ism.get_internal_dart(b);
            auto cd = ism.get_internal_dart(c);
            auto dd = ism.get_internal_dart(d);
            auto ed = ism.get_internal_dart(e);
            auto fd = ism.get_internal_dart(f);

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
            auto ad = ism.get_internal_dart(a);
            auto bd = ism.get_internal_dart(b);
            auto cd = ism.get_internal_dart(c);
            auto dd = ism.get_internal_dart(d);
            auto ed = ism.get_internal_dart(e);
            auto fd = ism.get_internal_dart(f);

            CHECK((ad == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d012.permutation())));
            CHECK((bd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d021.permutation())));
            CHECK((cd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d102.permutation())));
            CHECK((dd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d120.permutation())));
            CHECK((ed == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d201.permutation())));
            CHECK((fd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d210.permutation())));
        }
    }
}

TEST_CASE("index_simplex_mesh_mapper", "[simplex][indexing][dart][.]")
{
    {
        wmtk::MatrixXl S(3, 2);

        S.row(0) << 0, 1;
        S.row(1) << 2, 3;
        S.row(2) << 1, 2;

        wmtk::EdgeMesh em;
        em.initialize(S);

        wmtk::utils::internal::IndexSimplexMapper ism(em);


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
        wmtk::TriMesh em;
        em.initialize(S);

        wmtk::utils::internal::IndexSimplexMapper ism(em);


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
            auto ad = ism.get_internal_dart(a);
            auto bd = ism.get_internal_dart(b);
            auto cd = ism.get_internal_dart(c);
            auto dd = ism.get_internal_dart(d);
            auto ed = ism.get_internal_dart(e);
            auto fd = ism.get_internal_dart(f);

            CHECK((ad == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d012.permutation())));
            CHECK((bd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d021.permutation())));
            CHECK((cd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d102.permutation())));
            CHECK((dd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d120.permutation())));
            CHECK((ed == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d201.permutation())));
            CHECK((fd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d210.permutation())));
        }
        {
            std::array<int64_t, 2> a{{0, 1}};
            std::array<int64_t, 2> b{{0, 2}};
            std::array<int64_t, 2> c{{1, 2}};
            std::array<int64_t, 2> d{{1, 3}};
            std::array<int64_t, 2> e{{2, 3}};

            std::array<int64_t, 2> A{{1, 0}};
            std::array<int64_t, 2> B{{2, 0}};
            std::array<int64_t, 2> C{{2, 1}};
            std::array<int64_t, 2> D{{3, 1}};
            std::array<int64_t, 2> E{{3, 2}};
            auto ad = ism.get_internal_dart(a);
            auto bd = ism.get_internal_dart(b);
            auto cd = ism.get_internal_dart(c);
            auto dd = ism.get_internal_dart(d);
            auto ed = ism.get_internal_dart(e);

            auto Ad = ism.get_internal_dart(A);
            auto Bd = ism.get_internal_dart(B);
            auto Cd = ism.get_internal_dart(C);
            auto Dd = ism.get_internal_dart(D);
            auto Ed = ism.get_internal_dart(E);

            CHECK(ad.global_id() == 0);
            CHECK(bd.global_id() == 1);
            CHECK(cd.global_id() == 2);
            CHECK(dd.global_id() == 3);
            CHECK(ed.global_id() == 4);

            CHECK(Ad.global_id() == 0);
            CHECK(Bd.global_id() == 1);
            CHECK(Cd.global_id() == 2);
            CHECK(Dd.global_id() == 3);
            CHECK(Ed.global_id() == 4);


            CHECK(ad.permutation() == wmtk::tests::dart::utils::d01.permutation());
            CHECK(bd.permutation() == wmtk::tests::dart::utils::d01.permutation());
            CHECK(cd.permutation() == wmtk::tests::dart::utils::d01.permutation());
            CHECK(dd.permutation() == wmtk::tests::dart::utils::d01.permutation());
            CHECK(ed.permutation() == wmtk::tests::dart::utils::d01.permutation());

            CHECK(Ad.permutation() == wmtk::tests::dart::utils::d10.permutation());
            CHECK(Bd.permutation() == wmtk::tests::dart::utils::d10.permutation());
            CHECK(Cd.permutation() == wmtk::tests::dart::utils::d10.permutation());
            CHECK(Dd.permutation() == wmtk::tests::dart::utils::d10.permutation());
            CHECK(Ed.permutation() == wmtk::tests::dart::utils::d10.permutation());
            /*
            CHECK((ad == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d01.permutation())));
            CHECK((bd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d01.permutation())));
            CHECK((cd == wmtk::dart::Dart(2, wmtk::tests::dart::utils::d01.permutation())));
            CHECK((dd == wmtk::dart::Dart(3, wmtk::tests::dart::utils::d01.permutation())));
            CHECK((ed == wmtk::dart::Dart(4, wmtk::tests::dart::utils::d01.permutation())));

            CHECK((Ad == wmtk::dart::Dart(0, wmtk::tests::dart::utils::d10.permutation())));
            CHECK((Bd == wmtk::dart::Dart(1, wmtk::tests::dart::utils::d10.permutation())));
            CHECK((Cd == wmtk::dart::Dart(2, wmtk::tests::dart::utils::d10.permutation())));
            CHECK((Dd == wmtk::dart::Dart(3, wmtk::tests::dart::utils::d10.permutation())));
            CHECK((Ed == wmtk::dart::Dart(4, wmtk::tests::dart::utils::d10.permutation())));
            */
        }
    }
}
