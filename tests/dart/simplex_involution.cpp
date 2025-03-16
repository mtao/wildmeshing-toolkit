#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/utils/apply_simplex_involution.hpp>
#include <wmtk/dart/utils/get_simplex_dart.hpp>
#include <wmtk/dart/utils/get_simplex_involution.hpp>
using namespace wmtk;
using namespace wmtk::dart;

TEST_CASE("simplex_adjacency_io", "[dart]")
{
    for (int j = 0; j < 4; ++j) {
        const SimplexDart& sa = wmtk::dart::utils::get_simplex_dart(j);
        auto pta = sa.simplex_type();
        for (int k = 0; k < 4; ++k) {
            const SimplexDart& sb = wmtk::dart::utils::get_simplex_dart(k);
            auto ptb = sb.simplex_type();

            for (int8_t aperm : sa.permutation_indices()) {
                for (int8_t bperm : sb.permutation_indices()) {
                    int8_t action_a = dart::utils::get_simplex_involution(pta, aperm, ptb, bperm);
                    int8_t action_b = dart::utils::get_simplex_involution(ptb, bperm, pta, aperm);

                }
            }
        }
    }
}
