

#include <catch2/catch.hpp>
#include "Models.h"

TEST_CASE("seamed_model_loading", "[models]")
{
    auto models = extremeopt::tests::get_all_model_files();
    REQUIRE(models.size() > 0);

    CHECK(models.find("helmet") != models.end());
    CHECK(models.find("femur") != models.end());
    CHECK(models.find("elk") != models.end());
    CHECK(models.find("genus3") != models.end());

    for(const auto& [name, paths]: models) {
        CHECK(std::filesystem::exists(paths.obj_filepath));
        if(paths.constraint_filepath) {
            CHECK(std::filesystem::exists(*paths.constraint_filepath));
        }
    }
}
