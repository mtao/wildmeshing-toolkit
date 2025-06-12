#include <catch2/catch_test_macros.hpp>
#include <wmtk/dart/utils/apply_simplex_involution.hpp>
#include <wmtk/dart/utils/get_simplex_dart.hpp>
#include <wmtk/dart/utils/get_simplex_involution.hpp>
#include "utils/canonical_darts.hpp"
using namespace wmtk;
using namespace wmtk::dart;

TEST_CASE("dart_action_invertibility", "[dart]")
{
    for (int j = 0; j < 4; ++j) {
        const SimplexDart& sa = wmtk::dart::utils::get_simplex_dart(j);
        auto pta = sa.simplex_type();
        for (int k = 0; k < 4; ++k) {
            const SimplexDart& sb = wmtk::dart::utils::get_simplex_dart(k);
            auto ptb = sb.simplex_type();

            for (const auto& aperm_d : wmtk::tests::dart::utils::darts(j)) {
                int8_t aperm = aperm_d.permutation();
                for (const auto& bperm_d : wmtk::tests::dart::utils::darts(k)) {
                    int8_t bperm = bperm_d.permutation();
                    int8_t action_a = dart::utils::get_simplex_involution(pta, aperm, ptb, bperm);
                    int8_t bperm2 =
                        wmtk::dart::utils::apply_simplex_involution(pta, ptb, action_a, aperm);
                    CHECK(bperm == bperm2);

                    int8_t action_b = dart::utils::get_simplex_involution(ptb, bperm, pta, aperm);
                    int8_t aperm2 =
                        dart::utils::apply_simplex_involution(ptb, pta, action_b, bperm);
                    CHECK(aperm == aperm2);

                    if (j <= k) {
                        if (j < k) {
                            CHECK(action_a == action_b);
                        }
                        int8_t inv_action = wmtk::dart::utils::apply_simplex_involution(
                            ptb,
                            pta,
                            action_a,
                            action_b);
                        CHECK(inv_action == sa.identity());
                        for (const auto& d : wmtk::tests::dart::utils::darts(j)) {
                            int8_t a_p = d.permutation();
                            int8_t b_p = wmtk::dart::utils::apply_simplex_involution(
                                pta,
                                ptb,
                                action_a,
                                a_p);
                            int8_t a_p2 = wmtk::dart::utils::apply_simplex_involution(
                                ptb,
                                pta,
                                action_b,
                                b_p);
                            CHECK(a_p == a_p2);
                        }
                    }
                }
            }
        }
    }
}
