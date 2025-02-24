#include <fmt/ranges.h>
#include <spdlog/spdlog.h>
#include <catch2/catch_test_macros.hpp>
#include <nlohmann/json.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/components/input/input.hpp>
#include "tools/TriMesh_examples.hpp"
#include "wmtk/components/multimesh/MeshCollection.hpp"
#include "wmtk/components/multimesh/MultimeshOptions.hpp"
#include "wmtk/components/multimesh/from_tag.hpp"

#include "utils.hpp"

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
        {"tag_attribute",
         {
             {"path", "/tag"},
             {"simplex_dimension", 1},
             {"type", "char"},
         }},
        {"value", 1}};

    wmtk::components::multimesh::MultimeshTagOptions opt = tag_js;
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

TEST_CASE("multimesh_boundary_json", "[components][multimesh]")
{
    using AT = wmtk::attribute::AttributeType;
    using AD = wmtk::components::multimesh::utils::AttributeDescription;

    using JS = nlohmann::json;
    JS tag_js = {
        {"type", "tag"},
        {"output_mesh_name", "tagged_edges"},
        {"tag_attribute",
         {
             {"path", "/tag"},
             {"simplex_dimension", 1},
             {"type", "char"},
         }},
        {"value", 1}};

    wmtk::components::multimesh::MultimeshTagOptions opt = tag_js;
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
}

// TEST_CASE("named_multimesh_parse_attributes", "[components][multimesh]") {}

