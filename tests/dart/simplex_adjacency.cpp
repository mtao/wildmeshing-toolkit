#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/SimplexAdjacency.hpp>
using namespace wmtk;
using namespace wmtk::dart;

namespace {

template <int D>
void check_sa_dim()
{
    static_assert(sizeof(SimplexAdjacency<D>) <= sizeof(int64_t) * (D + 1));
}
} // namespace
TEST_CASE("simplex_adjacency_io", "[dart]")
{
    SimplexAdjacency<3> sa;
    check_sa_dim<1>();
    check_sa_dim<2>();
    check_sa_dim<3>();

    sa[0] = Dart(-1, 1);
    sa[1] = Dart(0, 2);
    sa[2] = Dart(5, 3);


    CHECK(sa[0].global_id() == -1);
    CHECK(sa[1].global_id() == 0);
    CHECK(sa[2].global_id() == 5);
    CHECK(sa[0].permutation() == 1);
    CHECK(sa[1].permutation() == 2);
    CHECK(sa[2].permutation() == 3);

    auto saf = sa[0];
    CHECK(saf.global_id() == -1);
    CHECK(saf.permutation() == 1);

    saf = Dart(20, 30);
    CHECK(sa[0].global_id() == 20);
    CHECK(sa[0].permutation() == 30);

    int count = 0;
    for(const auto& dw: sa) {

        Dart dw2 = sa[count++];
        CHECK(dw2 == dw);
    }
    CHECK(count == 3);

}
