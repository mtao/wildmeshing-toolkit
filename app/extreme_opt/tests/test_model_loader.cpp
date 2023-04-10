

#include <catch2/catch.hpp>
#include "ExtremeOpt.h"
#include "Models.h"
#include <spdlog/spdlog.h>

TEST_CASE("seamed_model_loading", "[models]")
{
    auto models = extremeopt::tests::get_all_model_files();
    REQUIRE(models.size() > 0);

    CHECK(models.find("helmet") != models.end());
    CHECK(models.find("femur") != models.end());
    CHECK(models.find("elk") != models.end());
    CHECK(models.find("genus3") != models.end());

    for(const auto& [name, paths]: models) {
        spdlog::info("Trying out model {}", name);
        CHECK(std::filesystem::exists(paths.obj_filepath));
        if(paths.constraint_filepath) {
            CHECK(std::filesystem::exists(*paths.constraint_filepath));

        }
        extremeopt::ExtremeOpt opt;
        extremeopt::tests::create(name,opt);
        CHECK(opt.tri_capacity() > 0);
        CHECK(opt.vert_capacity() > 0);
        if(paths.constraint_filepath) {
            CHECK(opt.edge_attrs.size() > 0);
        }

    }
}
