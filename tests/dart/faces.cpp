#include <fmt/ranges.h>
#include <catch2/catch_test_macros.hpp>
#include <set>
#include <wmtk/dart/SimplexDart.hpp>
#include <wmtk/dart/utils/get_canonical_faces.hpp>
#include <wmtk/dart/utils/get_cofaces.hpp>
#include <wmtk/dart/utils/get_faces.hpp>
#include <wmtk/dart/utils/get_local_vertex_permutation.hpp>
#include "utils/canonical_darts.hpp"
using namespace wmtk;
using namespace wmtk::dart;
using namespace wmtk::tests;

constexpr PrimitiveType PV = PrimitiveType::Vertex;
constexpr PrimitiveType PE = PrimitiveType::Edge;
constexpr PrimitiveType PF = PrimitiveType::Triangle;
constexpr PrimitiveType PT = PrimitiveType::Tetrahedron;

namespace {
int64_t factorial(int64_t n)
{
    if (n <= 1) {
        return 1;
    } else {
        return factorial(n - 1) * n;
    }
}
} // namespace

TEST_CASE("canonical_faces", "[tuple]")
{
    auto run = [](const PrimitiveType mesh_pt, const PrimitiveType pt) {
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
        auto E = wmtk::dart::utils::get_canonical_faces(sd, pt);
        int8_t mesh_dim = get_primitive_type_id(mesh_pt) + 1;
        int8_t dim = get_primitive_type_id(pt) + 1;
        REQUIRE(E.size() == factorial(mesh_dim) / (factorial(mesh_dim - dim) * factorial(dim)));
        for (int8_t j = 0; j < E.size(); ++j) {
            CHECK(sd.simplex_index(E[j], pt) == j);
        }
    };
    run(PE, PV);
    run(PE, PE);

    run(PF, PV);
    run(PF, PE);
    run(PF, PF);

    run(PT, PV);
    run(PT, PE);
    run(PT, PF);
    run(PT, PT);
}

TEST_CASE("faces", "[tuple]")
{
    auto run = [](const PrimitiveType mesh_pt,
                  const PrimitiveType simplex_pt,
                  const PrimitiveType face_pt) {
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
        int8_t simplex_dim = get_primitive_type_id(simplex_pt) + 1;
        int8_t dim = get_primitive_type_id(face_pt) + 1;
        int64_t num_faces =
            factorial(simplex_dim) / (factorial(simplex_dim - dim) * factorial(dim));
        for (int8_t p = 0; p < sd.size(); ++p) {
            auto full_indices = wmtk::dart::utils::get_local_vertex_permutation(mesh_pt, p);
            std::set<int8_t> simplex_indices(
                full_indices.begin(),
                full_indices.begin() + simplex_dim);
            auto faces = wmtk::dart::utils::get_faces(mesh_pt, simplex_pt, p, face_pt);
            REQUIRE(faces.size() == num_faces);
            for (int8_t j = 0; j < faces.size(); ++j) {
                auto full_face_indices =
                    wmtk::dart::utils::get_local_vertex_permutation(mesh_pt, faces[j]);
                std::set<int8_t> face_indices(
                    full_face_indices.begin(),
                    full_face_indices.begin() + dim);
                for (const auto& f : face_indices) {
                    CHECK(simplex_indices.find(f) != simplex_indices.end());
                }
            }
        }
    };
    run(PE, PV, PV);
    run(PE, PE, PV);
    run(PE, PE, PE);

    run(PF, PV, PV);
    run(PF, PE, PV);
    run(PF, PE, PE);
    run(PF, PF, PV);
    run(PF, PF, PE);
    run(PF, PF, PF);

    run(PT, PV, PV);
    run(PT, PE, PV);
    run(PT, PE, PE);
    run(PT, PF, PV);
    run(PT, PF, PE);
    run(PT, PF, PF);
    run(PT, PT, PV);
    run(PT, PT, PE);
    run(PT, PT, PF);
    run(PT, PT, PT);
}

TEST_CASE("cofaces", "[tuple]")
{
    auto run = [](const PrimitiveType mesh_pt,
                  const PrimitiveType coface_pt,
                  const PrimitiveType simplex_pt) {
        const auto& sd = wmtk::dart::SimplexDart::get_singleton(mesh_pt);
        int8_t simplex_dim = get_primitive_type_id(simplex_pt) + 1;
        int8_t dim = get_primitive_type_id(coface_pt) + 1;
        for (int8_t p = 0; p < sd.size(); ++p) {
            auto full_indices = wmtk::dart::utils::get_local_vertex_permutation(mesh_pt, p);
            std::set<int8_t> simplex_indices(
                full_indices.begin(),
                full_indices.begin() + simplex_dim);
            auto cofaces = wmtk::dart::utils::get_cofaces(mesh_pt, simplex_pt, p, coface_pt);
            for (int8_t j = 0; j < cofaces.size(); ++j) {
                auto full_coface_indices =
                    wmtk::dart::utils::get_local_vertex_permutation(mesh_pt, cofaces[j]);
                std::set<int8_t> coface_indices(
                    full_coface_indices.begin(),
                    full_coface_indices.begin() + dim);
                for (const auto& f : simplex_indices) {
                    CHECK(simplex_indices.find(f) != simplex_indices.end());
                }
            }
        }
    };
    run(PE, PV, PV);
    run(PE, PE, PV);
    run(PE, PE, PE);

    run(PF, PV, PV);
    run(PF, PE, PV);
    run(PF, PE, PE);
    run(PF, PF, PV);
    run(PF, PF, PE);
    run(PF, PF, PF);

    run(PT, PV, PV);
    run(PT, PE, PV);
    run(PT, PE, PE);
    run(PT, PF, PV);
    run(PT, PF, PE);
    run(PT, PF, PF);
    run(PT, PT, PV);
    run(PT, PT, PE);
    run(PT, PT, PF);
    run(PT, PT, PT);
}
