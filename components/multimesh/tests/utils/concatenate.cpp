#include <catch2/catch_test_macros.hpp>
#include <wmtk/components/multimesh/utils/concatenate.hpp>


using namespace wmtk;

TEST_CASE("multimesh_concatenate_mat", "[components][multimesh]")
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

        std::vector vec{A, B};
        auto D = components::multimesh::utils::concatenate(vec);

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

        std::vector vec{A, B};
        auto D = components::multimesh::utils::concatenate(vec);

        CHECK(C == D);
    }
}

TEST_CASE("multimesh_concatenate_tuple", "[components][multimesh]")
{
    {
        std::vector<std::array<Tuple, 2>> A;
        A.emplace_back(std::array<Tuple, 2>{{Tuple(0, 1, 2, 0), Tuple(0, 1, 2, 0)}});
        A.emplace_back(std::array<Tuple, 2>{{Tuple(0, 1, 2, 1), Tuple(0, 1, 2, 0)}});

        std::vector<std::array<Tuple, 2>> B;
        B.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 0), Tuple(0, 1, 2, 0)}});
        B.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 1), Tuple(0, 1, 2, 0)}});
        B.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 2), Tuple(0, 1, 2, 0)}});
        B.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 3), Tuple(0, 1, 2, 0)}});

        std::vector<std::array<Tuple, 2>> C;
        C.emplace_back(std::array<Tuple, 2>{{Tuple(0, 1, 2, 0), Tuple(0, 1, 2, 0)}});
        C.emplace_back(std::array<Tuple, 2>{{Tuple(0, 1, 2, 1), Tuple(0, 1, 2, 0)}});
        C.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 2), Tuple(0, 1, 2, 0)}});
        C.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 3), Tuple(0, 1, 2, 0)}});
        C.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 4), Tuple(0, 1, 2, 0)}});
        C.emplace_back(std::array<Tuple, 2>{{Tuple(0, 2, 2, 5), Tuple(0, 1, 2, 0)}});

        std::vector vec{A, B};
        auto D = components::multimesh::utils::concatenate(vec);

        CHECK(C == D);
    }
}
