#include <numeric>
#include <wmtk/Tuple.hpp>

#include <spdlog/stopwatch.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/attribute/TupleAccessor.hpp>
#include <wmtk/dart/DartTopologyAccessor.hpp>
#include <wmtk/io/read_mesh.hpp>
#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include <wmtk/utils/Logger.hpp>
#include "tools/TriMesh_examples.hpp"
#include "tools/TetMesh_examples.hpp"
#include "tools/all_valid_local_tuples.hpp"
TEST_CASE("dart_access_quad", "[dart_accessor]")

{
    auto mesh = wmtk::tests::quad();

    auto handle = wmtk::dart::register_dart_boundary_topology_attribute(mesh, "dart", true);


    wmtk::dart::DartTopologyAccessor acc(mesh, handle);

    for (size_t j = 0; j < acc.size(); ++j) {
        const auto& sa = acc[j];
        for (const auto& t : sa) {
            fmt::print("{} ", std::string(t));
        }
        fmt::print("\n");
    }

    std::array<wmtk::Tuple, 6> at = {
        wmtk::Tuple(0, 1, -1, 0), // 0
        wmtk::Tuple(1, 2, -1, 0), // 1
        wmtk::Tuple(2, 0, -1, 0), // 2
        wmtk::Tuple(0, 2, -1, 0), // 3
        wmtk::Tuple(1, 0, -1, 0), // 4
        wmtk::Tuple(2, 1, -1, 0), // 5
    };

    std::array<wmtk::Tuple, 6> bt = {
        wmtk::Tuple(0, 1, -1, 1), // 0
        wmtk::Tuple(1, 2, -1, 1), // 1
        wmtk::Tuple(2, 0, -1, 1), // 2
        wmtk::Tuple(0, 2, -1, 1), // 3
        wmtk::Tuple(1, 0, -1, 1), // 4
        wmtk::Tuple(2, 1, -1, 1), // 5
    };

    const auto& sd = wmtk::dart::SimplexDart::get_singleton(wmtk::PrimitiveType::Triangle);
    std::array<wmtk::dart::Dart, 6> ad;

    std::transform(at.begin(), at.end(), ad.begin(), [&](const auto& a) {
        return sd.dart_from_tuple(a);
    });

    std::array<wmtk::dart::Dart, 6> bd;
    std::transform(bt.begin(), bt.end(), bd.begin(), [&](const auto& a) {
        return sd.dart_from_tuple(a);
    });

    auto do_tri_check = [&](int a, int b) {
        wmtk::dart::Dart mad = ad[a];
        wmtk::dart::Dart mbd = bd[b];
        wmtk::Tuple mat = at[a];
        wmtk::Tuple mbt = bt[b];
        REQUIRE((mesh.switch_face(mat) == mbt));
        REQUIRE((mesh.switch_face(mbt) == mat));
        spdlog::info(
            "{}=>{} dart {}=>{} switch got {} {}",
            std::string(mat),
            std::string(mbt),
            std::string(mad),
            std::string(mbd),
            std::string(acc.switch_facet(mad)),
            std::string(acc.switch_facet(mbd)));
        CHECK((acc.switch_facet(mad) == mbd));
        CHECK((acc.switch_facet(mbd) == mad));
    };

    do_tri_check(1, 4);
    do_tri_check(3, 2);
}

TEST_CASE("dart_access_three_neighbors", "[dart_accessor]")
{
    auto mesh = wmtk::tests::three_neighbors();

    auto handle = wmtk::dart::register_dart_boundary_topology_attribute(mesh, "dart", true);


    wmtk::dart::DartTopologyAccessor acc(mesh, handle);
    REQUIRE(mesh.get_all(wmtk::PrimitiveType::Triangle).size() == 4);
    REQUIRE(acc.m_base_accessor.reserved_size() == 4);
    REQUIRE(acc.m_base_accessor.dimension() == 4);
    REQUIRE(acc.size() == 4);

    for (size_t j = 0; j < acc.size(); ++j) {
        const auto& sa = acc[j];
        for (const auto& t : sa) {
            fmt::print("{} ", std::string(t));
        }
        fmt::print("\n");
    }

    const auto& sd = wmtk::dart::SimplexDart::get_singleton(wmtk::PrimitiveType::Triangle);


    for (size_t j = 0; j < 4; ++j) {
        for (const wmtk::Tuple& t :
             wmtk::tests::all_valid_local_tuples(wmtk::PrimitiveType::Triangle, j)) {
            // for (const wmtk::Tuple& t : mesh.get_all(wmtk::PrimitiveType::Edge)) {
            wmtk::dart::Dart d = sd.dart_from_tuple(t);
            for (wmtk::PrimitiveType pt :
                 {wmtk::PrimitiveType::Vertex, wmtk::PrimitiveType::Edge}) {
                wmtk::Tuple ot = mesh.switch_tuple(t, pt);
                auto od = acc.switch_dart(d, pt);
                wmtk::dart::Dart od2 = sd.dart_from_tuple(ot);
                CHECK(od.global_id() == od2.global_id());
                CHECK(od.permutation() == od2.permutation());
            }
            bool is_boundary_m = mesh.is_boundary(wmtk::PrimitiveType::Edge, t);
            bool is_boundary_d = acc.is_boundary(d);
            REQUIRE(is_boundary_m == is_boundary_d);
            if (!is_boundary_m) {
                wmtk::Tuple ot = mesh.switch_tuple(t, wmtk::PrimitiveType::Triangle);
                auto od = acc.switch_dart(d, wmtk::PrimitiveType::Triangle);
                wmtk::dart::Dart od2 = sd.dart_from_tuple(ot);
                spdlog::warn(
                    "{}/{} {}/{}",
                    std::string(d),
                    sd.simplex_index(d, wmtk::PrimitiveType::Edge),
                    std::string(od),
                    sd.simplex_index(od, wmtk::PrimitiveType::Edge));
                CHECK(od.global_id() == od2.global_id());
                CHECK(od.permutation() == od2.permutation());

                auto od3 = acc.switch_dart(od, wmtk::PrimitiveType::Triangle);
                CHECK(d.global_id() == od3.global_id());
                CHECK(d.permutation() == od3.permutation());
            }
        }
    }
}

TEST_CASE("dart_access_two_by_three_grids_tests", "[dart_accessor]")
{
    auto mesh = wmtk::tests_3d::two_by_three_grids_tets();

    auto handle = wmtk::dart::register_dart_boundary_topology_attribute(mesh, "dart", true);


    wmtk::dart::DartTopologyAccessor acc(mesh, handle);
    REQUIRE(mesh.get_all(wmtk::PrimitiveType::Tetrahedron).size() == 30);
    REQUIRE(acc.m_base_accessor.reserved_size() == 30);
    REQUIRE(acc.m_base_accessor.dimension() == 5);
    REQUIRE(acc.size() == 30);

    for (size_t j = 0; j < acc.size(); ++j) {
        const auto& sa = acc[j];
        for (const auto& t : sa) {
            fmt::print("{} ", std::string(t));
        }
        fmt::print("\n");
    }

    const auto& sd = wmtk::dart::SimplexDart::get_singleton(wmtk::PrimitiveType::Tetrahedron);


    for (size_t j = 0; j < 4; ++j) {
        for (const wmtk::Tuple& t :
             wmtk::tests::all_valid_local_tuples(wmtk::PrimitiveType::Tetrahedron, j)) {
            // for (const wmtk::Tuple& t : mesh.get_all(wmtk::PrimitiveType::Edge)) {
            wmtk::dart::Dart d = sd.dart_from_tuple(t);
            for (wmtk::PrimitiveType pt :
                 {wmtk::PrimitiveType::Vertex, wmtk::PrimitiveType::Edge, wmtk::PrimitiveType::Triangle}) {
                wmtk::Tuple ot = mesh.switch_tuple(t, pt);
                auto od = acc.switch_dart(d, pt);
                wmtk::dart::Dart od2 = sd.dart_from_tuple(ot);
                CHECK(od.global_id() == od2.global_id());
                CHECK(od.permutation() == od2.permutation());
            }
            bool is_boundary_m = mesh.is_boundary(wmtk::PrimitiveType::Triangle, t);
            bool is_boundary_d = acc.is_boundary(d);
            REQUIRE(is_boundary_m == is_boundary_d);
            if (!is_boundary_m) {
                wmtk::Tuple ot = mesh.switch_tuple(t, wmtk::PrimitiveType::Tetrahedron);
                auto od = acc.switch_dart(d, wmtk::PrimitiveType::Tetrahedron);
                wmtk::dart::Dart od2 = sd.dart_from_tuple(ot);
                spdlog::warn(
                    "{}/{} {}/{}",
                    std::string(d),
                    sd.simplex_index(d, wmtk::PrimitiveType::Triangle),
                    std::string(od),
                    sd.simplex_index(od, wmtk::PrimitiveType::Triangle));
                CHECK(od.global_id() == od2.global_id());
                CHECK(od.permutation() == od2.permutation());

                auto od3 = acc.switch_dart(od, wmtk::PrimitiveType::Tetrahedron);
                CHECK(d.global_id() == od3.global_id());
                CHECK(d.permutation() == od3.permutation());
            }
        }
    }
}

const std::filesystem::path data_dir = WMTK_DATA_DIR;
TEST_CASE("dart_performance", "[performance]")
{
    const std::filesystem::path meshfile = data_dir / "armadillo.msh";
#if defined(NDEBUG)
    const int64_t iterations = 100;
#else
    const int64_t iterations = 10;
#endif

    auto mesh_in = std::static_pointer_cast<wmtk::TriMesh>(wmtk::io::read_mesh(meshfile));
    wmtk::TriMesh& mesh = *mesh_in;
    auto handle = wmtk::dart::register_dart_boundary_topology_attribute(mesh, "dart", true);
    std::vector<wmtk::Tuple> all_tuples = mesh.get_all(wmtk::PrimitiveType::Edge);
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(wmtk::PrimitiveType::Triangle);
    std::vector<wmtk::dart::Dart> all_darts;
    for (const auto& t : all_tuples) {
        all_darts.emplace_back(sd.dart_from_tuple(t));
    }
    {
        spdlog::stopwatch sw;
        auto test_handle =
            mesh.register_attribute<int64_t>("Test attr", wmtk::PrimitiveType::Edge, 1, true);
        wmtk::dart::DartTopologyAccessor acc(mesh, handle);

        auto test_acc = mesh.create_accessor<int64_t, 1>(test_handle);
        for (int j = 0; j < iterations; ++j) {
            for (const wmtk::dart::Dart& d : all_darts) {
                bool is_boundary_d = acc.is_boundary(d);
                if (!is_boundary_d) {
                    auto od = acc.switch_dart(d, wmtk::PrimitiveType::Triangle);
                    test_acc.scalar_attribute(od) = od.global_id();
                }
            }
        }
        spdlog::info("Dart Elapsed: {} seconds", sw);
    }
    {
        spdlog::stopwatch sw;
        auto test_handle =
            mesh.register_attribute<int64_t>("Test attr", wmtk::PrimitiveType::Edge, 1, true);
        wmtk::dart::DartTopologyAccessor acc(mesh, handle);

        auto test_acc = mesh.create_accessor<int64_t, 1>(test_handle);
        for (int j = 0; j < iterations; ++j) {
            for (const wmtk::Tuple& t : all_tuples) {
                wmtk::dart::Dart d = sd.dart_from_tuple(t);

                bool is_boundary_d = acc.is_boundary(d);
                if (!is_boundary_d) {
                    auto od = acc.switch_dart(d, wmtk::PrimitiveType::Triangle);
                    wmtk::Tuple ot = sd.tuple_from_dart(od);
                    test_acc.scalar_attribute(ot) = od.global_id();
                }
            }
        }
        spdlog::info("Dart from Tuple Elapsed: {} seconds", sw);
    }

    {
        spdlog::stopwatch sw;
        auto test_handle =
            mesh.register_attribute<int64_t>("Test attr", wmtk::PrimitiveType::Edge, 1, true);

        auto test_acc = mesh.create_accessor<int64_t, 1>(test_handle);

        for (int j = 0; j < iterations; ++j) {
            for (const wmtk::Tuple& t : all_tuples) {
                bool is_boundary_m = mesh.is_boundary(wmtk::PrimitiveType::Edge, t);
                if (!is_boundary_m) {
                    wmtk::Tuple ot = mesh.switch_tuple(t, wmtk::PrimitiveType::Triangle);
                    test_acc.scalar_attribute(ot) = ot.global_cid();
                }
            }
        }
        spdlog::info("Tuple Elapsed: {} seconds", sw);
    }
}
