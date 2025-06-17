#include <fmt/ranges.h>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/input/input.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/io/read_mesh.hpp>
#include "tools/TriMesh_examples.hpp"
#include "wmtk/components/multimesh/MeshCollection.hpp"
#include "wmtk/components/multimesh/MultimeshOptions.hpp"
#include "wmtk/components/multimesh/from_tag.hpp"

#include "utils.hpp"
namespace fs = std::filesystem;
const fs::path wmtk_data_dir = WMTK_DATA_DIR;

using json = nlohmann::json;
using AT = wmtk::attribute::AttributeType;

using AD = wmtk::components::multimesh::utils::AttributeDescription;
namespace {
// hack to deal with constructor ambiguity in the AD class
auto make_AD = [](const std::string_view& name,
                  const std::optional<uint8_t>& dim,
                  const std::optional<AT>& at) { return AD{name, dim, at}; };

} // namespace

TEST_CASE("multimesh_tag_json", "[components][multimesh]")
{
    using AT = wmtk::attribute::AttributeType;
    using AD = wmtk::components::multimesh::utils::AttributeDescription;

    using JS = nlohmann::json;
    JS tag_js = {
        {"type", "tag"},
        {"output_mesh_name", "tagged_edges"},
        {"manifold_decomposition", false},
        {"tag_attribute",
         {
             {"path", "/tag"},
             {"simplex_dimension", 1},
             {"type", "char"},
         }},
        {"value", 1}};

    wmtk::components::multimesh::MultimeshOptions opt = tag_js;
    REQUIRE(
        std::dynamic_pointer_cast<wmtk::components::multimesh::MultimeshTagOptions>(opt.options) !=
        nullptr);
    wmtk::components::multimesh::MeshCollection mc;
    auto mptr = wmtk::tests::disk(10);
    mc.emplace_mesh(*mptr, std::string("root"));
    auto tag_attr = mptr->register_attribute_typed<char>("tag", wmtk::PrimitiveType::Edge, 1);
    auto tag_acc = mptr->create_accessor(tag_attr);
    for (const auto& t : mptr->get_all(wmtk::PrimitiveType::Edge)) {
        if (mptr->is_boundary_edge(t)) {
            tag_acc.scalar_attribute(t) = 1;
        }
    }
    opt.run(mc);
    const auto& nmm = mc.get_named_multimesh("root");

    auto& ring = mc.get_mesh("root.tagged_edges");
    CHECK(ring.get_all(wmtk::PrimitiveType::Vertex).size() == 10);
    CHECK(ring.get_all(wmtk::PrimitiveType::Edge).size() == 10);
    REQUIRE(ring.absolute_multi_mesh_id() == std::vector<int64_t>{0});
    REQUIRE(&ring.get_multi_mesh_root() == mptr.get());
    for (const auto& t : ring.get_all(wmtk::PrimitiveType::Edge)) {
        // every edge in this ring should be interior
        CHECK(!ring.is_boundary(wmtk::PrimitiveType::Edge, t));
        auto ps = ring.map_to_parent(wmtk::simplex::Simplex::edge(t));
        REQUIRE(mptr->is_valid(ps));
        CHECK(mptr->is_boundary(ps));
    }
    {
        wmtk::components::multimesh::MultimeshOptions gen_opts;
        gen_opts = tag_js;
        auto ptr = std::dynamic_pointer_cast<wmtk::components::multimesh::MultimeshTagOptions>(
            gen_opts.options);
        REQUIRE(bool(ptr));
        CHECK((*ptr) == opt);
    }
}
TEST_CASE("multimesh_tag_creation_json", "[components][multimesh]")
{
    using AT = wmtk::attribute::AttributeType;
    using AD = wmtk::components::multimesh::utils::AttributeDescription;

    using JS = nlohmann::json;
    JS tag_js = {
        {"type", "tag"},
        {"output_mesh_name", "tagged_edges"},
        {"manifold_decomposition", false},
        {"creation_attributes",
         {{{"attribute_path", "main/normal"},
           {"base_attribute_path", "main/vertices"},
           {"type", "normal"}},
          {{"attribute_path", "main/edge_angle"},
           {"base_attribute_path", "main/normal"},
           {"type", "edge_angle"}},
          {{"attribute_path", "main/high_edge_angle"},
           {"base_attribute_path", "main/edge_angle"},
           {"parameters", {{"over", 1.5}}},
           {"type", "threshold"}}}},
        {"tag_attribute",
         {
             {"path", "/high_edge_angle"},
             {"simplex_dimension", 1},
             {"type", "char"},
         }},
        {"value", 1}};

    wmtk::components::multimesh::MultimeshOptions opt = tag_js;
    REQUIRE(
        std::dynamic_pointer_cast<wmtk::components::multimesh::MultimeshTagOptions>(opt.options) !=
        nullptr);
    wmtk::components::multimesh::MeshCollection mc;
    std::shared_ptr<wmtk::Mesh> mptr = wmtk::io::read_mesh(WMTK_DATA_DIR "/100071_sf.msh");
    mc.emplace_mesh(*mptr, std::string("root"));
}

TEST_CASE("multimesh_boundary_json", "[components][multimesh]")
{
    using AT = wmtk::attribute::AttributeType;
    using AD = wmtk::components::multimesh::utils::AttributeDescription;

    using JS = nlohmann::json;

    auto run = [](const auto& js) {
        wmtk::components::multimesh::MultimeshOptions opt = js;

        REQUIRE(
            std::dynamic_pointer_cast<wmtk::components::multimesh::MultimeshBoundaryOptions>(
                opt.options) != nullptr);
        wmtk::components::multimesh::MeshCollection mc;
        auto mptr = wmtk::tests::disk(10);
        mc.emplace_mesh(*mptr, std::string("root"));
        opt.run(mc);
        const auto& nmm = mc.get_named_multimesh("root");

        auto& ring = mc.get_mesh("root.boundary");
        CHECK(ring.get_all(wmtk::PrimitiveType::Vertex).size() == 10);
        CHECK(ring.get_all(wmtk::PrimitiveType::Edge).size() == 10);
        REQUIRE(ring.absolute_multi_mesh_id() == std::vector<int64_t>{0});
        REQUIRE(&ring.get_multi_mesh_root() == mptr.get());
        for (const auto& t : ring.get_all(wmtk::PrimitiveType::Edge)) {
            // every edge in this ring should be interior
            CHECK(!ring.is_boundary(wmtk::PrimitiveType::Edge, t));
            auto ps = ring.map_to_parent(wmtk::simplex::Simplex::edge(t));
            REQUIRE(mptr->is_valid(ps));
            CHECK(mptr->is_boundary(ps));
        }
        return mc;
    };
    {
        JS tag_js = {
            {"type", "boundary"},
            {"mesh_path", "root"},
            {"output_mesh_name", "boundary"},
            {"dimension", 1},
            {"attribute_name", "is_boundary"},
            {"attribute_value", 5}};
        auto mc = run(tag_js);
        auto mah = wmtk::components::multimesh::utils::get_attribute(
            mc,
            wmtk::components::multimesh::utils::AttributeDescription("root/is_boundary"));
        REQUIRE(mah.is_valid());
        CHECK(mah.dimension() == 1);
        CHECK(mah.primitive_type() == wmtk::PrimitiveType::Edge);
        CHECK(mah.held_type() == wmtk::attribute::AttributeType::Char);
    }
    {
        JS tag_js = {{"type", "boundary"}, {"output_mesh_name", "boundary"}, {"dimension", 1}};
        auto mc = run(tag_js);
        CHECK_THROWS(wmtk::components::multimesh::utils::get_attribute(
            mc,
            wmtk::components::multimesh::utils::AttributeDescription("root/_boundary_tag")));
    }
}

TEST_CASE("multimesh_bijection_json", "[components][multimesh]")
{
    using AT = wmtk::attribute::AttributeType;
    using AD = wmtk::components::multimesh::utils::AttributeDescription;

    using JS = nlohmann::json;

    JS js = {{"type", "bijection"}, {"parent_path", "fused"}, {"child_path", "separated"}};
    wmtk::components::multimesh::MultimeshOptions opt = js;

    REQUIRE(
        std::dynamic_pointer_cast<wmtk::components::multimesh::MultimeshBijectionOptions>(
            opt.options) != nullptr);
    wmtk::components::multimesh::MeshCollection mc;
    auto mptr = wmtk::tests::disk(10);
    auto cmptr = wmtk::tests::individual_triangles(10);
    mc.emplace_mesh(*mptr, std::string("fused"));
    mc.emplace_mesh(*cmptr, std::string("separated"));
    opt.run(mc);

    auto& nmm = mc.get_named_multimesh("fused");
    auto& child = mc.get_mesh("fused.separated");
    REQUIRE(&child == cmptr.get());
    for (const auto& t : child.get_all(wmtk::PrimitiveType::Edge)) {
        auto t2s = child.map(*mptr, wmtk::simplex::Simplex::edge(t));
        REQUIRE(t2s.size() == 1);
        CHECK(t == t2s[0].tuple());
    }
}

TEST_CASE("multimesh_fusion_json", "[components][multimesh]")
{
    using AT = wmtk::attribute::AttributeType;
    using AD = wmtk::components::multimesh::utils::AttributeDescription;

    using JS = nlohmann::json;

    {
        JS js = {
            {"type", "axis_aligned_fusion"},
            {"attribute_path", "/vertices"},
            {"fused_mesh_name", "fused"},
            {"epsilon", 1e-5},
            {"axes_to_fuse", {true, true}}};
        wmtk::components::multimesh::MultimeshOptions opt = js;

        REQUIRE(
            std::dynamic_pointer_cast<
                wmtk::components::multimesh::MultimeshAxisAlignedFusionOptions>(opt.options) !=
            nullptr);
        wmtk::components::multimesh::MeshCollection mc;
        auto mptr = wmtk::tests::grid(5, 10);

        CHECK(mptr->get_all(wmtk::PrimitiveType::Vertex).size() == 50);
        CHECK(mptr->get_all(wmtk::PrimitiveType::Triangle).size() == 72);
        mc.emplace_mesh(*mptr, std::string("root"));
        opt.run(mc);

        auto& nmm = mc.get_named_multimesh("fused");
        auto& child = mc.get_mesh("fused.root");
        auto& root = mc.get_mesh("fused");
        CHECK(root.get_all(wmtk::PrimitiveType::Vertex).size() == 36);
        CHECK(root.get_all(wmtk::PrimitiveType::Triangle).size() == 72);
        REQUIRE(&child == mptr.get());
        for (const auto& t : child.get_all(wmtk::PrimitiveType::Edge)) {
            auto s = wmtk::simplex::Simplex::edge(t);
            {
                // map to root is unique
                auto t2s = child.map(root, s);
                REQUIRE(t2s.size() == 1);
            }

            // check self mapping
            auto t2s = child.map(*mptr, s);
            if (child.is_boundary(s)) {
                REQUIRE(t2s.size() == 2);
            } else {
                REQUIRE(t2s.size() == 1);
                CHECK(t == t2s[0].tuple());
            }
        }
    }
    {
        JS js = {
            {"type", "axis_aligned_fusion"},
            {"attribute_path", "/vertices"},
            {"fused_mesh_name", "fused"},
            {"epsilon", 1e-5},
            {"axes_to_fuse", {true, false}}};
        wmtk::components::multimesh::MultimeshOptions opt = js;

        REQUIRE(
            std::dynamic_pointer_cast<
                wmtk::components::multimesh::MultimeshAxisAlignedFusionOptions>(opt.options) !=
            nullptr);
        wmtk::components::multimesh::MeshCollection mc;
        auto mptr = wmtk::tests::grid(5, 10);
        mc.emplace_mesh(*mptr, std::string("root"));
        opt.run(mc);

        auto& nmm = mc.get_named_multimesh("fused");
        auto& root = mc.get_mesh("fused");
        CHECK(root.get_all(wmtk::PrimitiveType::Vertex).size() == 40);
        CHECK(root.get_all(wmtk::PrimitiveType::Triangle).size() == 72);
    }
    {
        JS js = {
            {"type", "axis_aligned_fusion"},
            {"attribute_path", "/vertices"},
            {"fused_mesh_name", "fused"},
            {"epsilon", 1e-5},
            {"axes_to_fuse", {false, true}}};
        wmtk::components::multimesh::MultimeshOptions opt = js;

        REQUIRE(
            std::dynamic_pointer_cast<
                wmtk::components::multimesh::MultimeshAxisAlignedFusionOptions>(opt.options) !=
            nullptr);
        wmtk::components::multimesh::MeshCollection mc;
        auto mptr = wmtk::tests::grid(5, 10);
        mc.emplace_mesh(*mptr, std::string("root"));
        opt.run(mc);

        auto& nmm = mc.get_named_multimesh("fused");
        auto& root = mc.get_mesh("fused");
        CHECK(root.get_all(wmtk::PrimitiveType::Vertex).size() == 45);
        CHECK(root.get_all(wmtk::PrimitiveType::Triangle).size() == 72);
    }
}

// TEST_CASE("named_multimesh_parse_attributes", "[components][multimesh]") {}

