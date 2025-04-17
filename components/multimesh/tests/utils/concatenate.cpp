#include <catch2/catch_test_macros.hpp>
#include <wmtk/components/multimesh/utils/concatenate.hpp>


using namespace wmtk;

TEST_CASE("multimesh_concatenate", "[components][multimesh]")
{
    {
        MatrixXl A(2, 2);
        A.row(0) << 0, 1;
        A.row(1) << 2, 3;
        MatrixXl B(4, 2);
        B.row(0) << 0, 1;
        B.row(1) << 2, 3;
        B.row(2) << 0, 3;
        B.row(3) << 2, 4;

        MatrixXl C(6, 2);
        C.row(0) << 0, 1;
        C.row(1) << 2, 3;
        C.row(2) << 4, 5;
        C.row(3) << 6, 7;
        C.row(4) << 4, 7;
        C.row(5) << 6, 8;

        auto D = components::multimesh::utils::concatenate({A, B});

        CHECK(C == D);
    }

    {
        MatrixXl A(2, 3);
        A.row(0) << 0, 1, 0;
        A.row(1) << 2, 3, 0;
        MatrixXl B(4, 3);
        B.row(0) << 0, 1, 1;
        B.row(1) << 2, 3, 1;
        B.row(2) << 0, 3, 1;
        B.row(3) << 2, 4, 1;

        MatrixXl C(6, 3);
        C.row(0) << 0, 1, 0;
        C.row(1) << 2, 3, 0;
        C.row(2) << 4, 5, 5;
        C.row(3) << 6, 7, 5;
        C.row(4) << 4, 7, 5;
        C.row(5) << 6, 8, 5;

        auto D = components::multimesh::utils::concatenate({A, B});

        CHECK(C == D);
    }
}
