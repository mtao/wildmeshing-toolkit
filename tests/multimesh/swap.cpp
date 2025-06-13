
#include <catch2/catch_test_macros.hpp>
#include <wmtk/utils/mesh_utils.hpp>

#include <catch2/catch_test_macros.hpp>
#include <wmtk/TriMeshOperationExecutor.hpp>
#include <wmtk/attribute/Accessor.hpp>
#include <wmtk/invariants/InteriorEdgeInvariant.hpp>
#include <wmtk/invariants/InteriorVertexInvariant.hpp>
#include <wmtk/invariants/MultiMeshLinkConditionInvariant.hpp>
#include <wmtk/multimesh/utils/internal/print_all_mapped_tuples.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>
#include <wmtk/operations/composite/TriFaceSplit.hpp>
#include <wmtk/simplex/link.hpp>
#include <wmtk/simplex/utils/SimplexComparisons.hpp>
#include <wmtk/utils/Logger.hpp>
#include <wmtk/utils/as_eigen_matrices.hpp>
#include <wmtk/utils/mesh_utils.hpp>
#include "tools/DEBUG_Mesh.hpp"
#include "tools/DEBUG_MultiMeshManager.hpp"
#include "tools/EdgeMesh_examples.hpp"
#include "tools/TriMesh_examples.hpp"

using namespace wmtk;
using namespace wmtk::simplex;
using namespace wmtk::tests;
using namespace operations;

using TM = TriMesh;
using MapResult = typename Eigen::Matrix<int64_t, Eigen::Dynamic, 1>::MapType;

constexpr PrimitiveType PV = PrimitiveType::Vertex;
constexpr PrimitiveType PE = PrimitiveType::Edge;
constexpr PrimitiveType PF = PrimitiveType::Triangle;
TEST_CASE("swap_multimesh_edge", "[operations][swap][2D]")
{
    using namespace wmtk;
    using namespace operations::composite;

    std::vector<std::array<Tuple, 2>> edges;
    edges.emplace_back(std::array<Tuple, 2>{{Tuple(1, -1, -1, 0), Tuple(1, 0, -1, 0)}});
    edges.emplace_back(std::array<Tuple, 2>{{Tuple(0, -1, -1, 1), Tuple(1, 2, -1, 1)}});

    VectorXl indices(4);
    indices(0) = 0;
    indices(1) = 1;
    indices(2) = 2;
    indices(3) = 3;
    auto print = [](const auto& m, auto& em) {
        auto F = wmtk::utils::as_eigen_matrices(m);
        auto E = wmtk::utils::as_eigen_matrices(em);
        auto EM = wmtk::utils::as_mapped_eigen_matrices(m, em);
        std::cout << "Mesh: \n" << F << std::endl;
        std::cout << "Child: \n" << E << std::endl;
        std::cout << "Child Mapped: \n" << EM << std::endl;
        const auto& mm = reinterpret_cast<const DEBUG_Mesh&>(m);
        mm.multi_mesh_manager().print(m);
    };
    {
        auto m = std::make_shared<TriMesh>(quad());
        auto em = std::make_shared<EdgeMesh>(tests::two_segments());
        m->register_child_mesh(em, edges);

        spdlog::info("Before op");
        print(*m, *em);

        REQUIRE(wmtk::simplex::utils::SimplexComparisons::equal(*m, PV, edges[0][1], edges[1][1]));


        REQUIRE(wmtk::simplex::utils::SimplexComparisons::equal(*em, PV, edges[0][0], edges[1][0]));


        Tuple vert_on_mm_t(1, 2, -1, 0);
        Tuple vert_opp_mm_t(0, 2, -1, 0);
        simplex::Simplex edge_to_mm(PrimitiveType::Edge, vert_on_mm_t);
        simplex::Simplex edge_opp_mm(PrimitiveType::Edge, vert_opp_mm_t);

        CHECK(m->is_boundary(PE, Tuple(1, 0, -1, 0)));
        CHECK(m->is_boundary(PE, Tuple(0, 1, -1, 0)));
        CHECK(!m->is_boundary(PE, Tuple(0, 2, -1, 0)));
        CHECK(!m->is_boundary(PE, Tuple(1, 0, -1, 1)));
        CHECK(m->is_boundary(PE, Tuple(0, 1, -1, 1)));
        CHECK(m->is_boundary(PE, Tuple(0, 2, -1, 1)));
        REQUIRE(!m->is_boundary(edge_to_mm));
        REQUIRE(!m->is_boundary(edge_opp_mm));
        REQUIRE(m->is_boundary(PrimitiveType::Edge, edges[0][1]));
        REQUIRE(m->is_boundary(PrimitiveType::Edge, edges[1][1]));

        {
            simplex::Simplex v(PrimitiveType::Vertex, vert_opp_mm_t);
            auto mapped_vs = m->map(*em, v);
            REQUIRE(mapped_vs.size() == 0);
        }
        {
            simplex::Simplex v(PrimitiveType::Vertex, vert_on_mm_t);
            auto mapped_vs = m->map(*em, v);
            REQUIRE(mapped_vs.size() == 1);
            for (const auto& mapped_v : mapped_vs) {
                CHECK(wmtk::simplex::utils::SimplexComparisons::equal(
                    *em,
                    PV,
                    mapped_v.tuple(),
                    edges[0][0]));
                CHECK(wmtk::simplex::utils::SimplexComparisons::equal(
                    *em,
                    PV,
                    mapped_v.tuple(),
                    edges[1][0]));
            }
        }
    }
    {
        auto m = std::make_shared<TriMesh>(quad());
        auto em = std::make_shared<EdgeMesh>(tests::two_segments());
        m->register_child_mesh(em, edges);
        spdlog::info("Before op");
        print(*m, *em);


        EdgeSplit es(*m);

        Tuple vert_on_mm_t(1, 2, -1, 0);
        simplex::Simplex edge_to_mm(PrimitiveType::Edge, vert_on_mm_t);
        es(edge_to_mm);
        spdlog::info("After split op");
        print(*m, *em);

        auto after_fs = m->get_all(PF);
        for (const auto& t : after_fs) {
            spdlog::info("{}", std::string(t));
        }
        for (const auto& t : em->get_all(PE)) {
            spdlog::info(
                "{} -> {}",
                std::string(t),
                std::string(em->map_to_root(simplex::Simplex::edge(t)).tuple()));
        }

        // make sure the two edges in the child mesh that point to each other still point to the
        // vertex
        {
            auto e0 = em->map_to_root(simplex::Simplex::vertex(edges[0][0]));
            auto e1 = em->map_to_root(simplex::Simplex::vertex(edges[1][0]));
            REQUIRE(wmtk::simplex::utils::SimplexComparisons::equal(*m, e0, e1));
        }
    }
    return;
    {
        auto m = std::make_shared<TriMesh>(quad());
        auto em = std::make_shared<EdgeMesh>(tests::two_segments());
        m->register_child_mesh(em, edges);
        spdlog::info("Before op");
        print(*m, *em);

        auto indices_handle = mesh_utils::set_matrix_attribute(
            indices,

            "index",
            wmtk::PrimitiveType::Vertex,
            *m);
        auto indices_acc = m->create_const_accessor<int64_t>(indices_handle);
        TriEdgeSwap op(*m);
        op.add_invariant(std::make_shared<InteriorEdgeInvariant>(*m));
        op.collapse().add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*m));


        op.collapse().set_new_attribute_strategy(indices_handle, CollapseBasicStrategy::CopyOther);
        op.split().set_new_attribute_strategy(indices_handle);

        Tuple vert_on_mm_t(1, 2, -1, 0);
        simplex::Simplex edge_to_mm(PrimitiveType::Edge, vert_on_mm_t);
        op(edge_to_mm);
        spdlog::info("After op");
        print(*m, *em);

        auto after_fs = m->get_all(PF);
        for (const auto& t : after_fs) {
            spdlog::info("{}", std::string(t));
        }
        for (const auto& t : em->get_all(PE)) {
            spdlog::info(
                "{} -> {}",
                std::string(t),
                std::string(em->map_to_root(simplex::Simplex::edge(t)).tuple()));
        }

        // make sure the two edges in the child mesh that point to each other still point to the
        // vertex
        {
            auto e0 = em->map_to_root(simplex::Simplex::vertex(edges[0][0]));
            auto e1 = em->map_to_root(simplex::Simplex::vertex(edges[1][0]));
            REQUIRE(wmtk::simplex::utils::SimplexComparisons::equal(*m, e0, e1));


            CHECK(indices_acc.const_scalar_attribute(e0) == 1);
            CHECK(indices_acc.const_scalar_attribute(e1) == 1);

            CHECK(indices_acc.const_scalar_attribute(m->switch_vertex(e0.tuple())) == 2);
            CHECK(indices_acc.const_scalar_attribute(m->switch_vertex(e1.tuple())) == 3);
            CHECK(
                indices_acc.const_scalar_attribute(
                    m->switch_tuples(e0.tuple(), {PV, PE, PF, PE, PV})) == 0);
        }
    }
    {
        auto m = std::make_shared<TriMesh>(quad());
        auto em = std::make_shared<EdgeMesh>(tests::two_segments());
        m->register_child_mesh(em, edges);

        auto indices_handle = mesh_utils::set_matrix_attribute(
            indices,

            "index",
            wmtk::PrimitiveType::Vertex,
            *m);
        auto indices_acc = m->create_const_accessor<int64_t>(indices_handle);
        TriEdgeSwap op(*m);
        op.add_invariant(std::make_shared<InteriorEdgeInvariant>(*m));
        op.collapse().add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*m));

        op.collapse().set_new_attribute_strategy(indices_handle, CollapseBasicStrategy::CopyOther);
        op.split().set_new_attribute_strategy(indices_handle);

        Tuple vert_opp_mm_t(0, 2, -1, 0);
        simplex::Simplex edge_opp_mm(PrimitiveType::Edge, vert_opp_mm_t);
        op(edge_opp_mm);
        auto after_fs = m->get_all(PF);
        for (const auto& t : after_fs) {
            spdlog::info("{}", std::string(t));
        }
        for (const auto& t : em->get_all(PE)) {
            spdlog::info(
                "{} -> {}",
                std::string(t),
                std::string(em->map_to_root(simplex::Simplex::edge(t)).tuple()));
        }

        // vertex
        // make sure the two edges in the child mesh that point to each other still point to the
        // vertex
        {
            auto e0 = em->map_to_root(simplex::Simplex::vertex(edges[0][0]));
            auto e1 = em->map_to_root(simplex::Simplex::vertex(edges[1][0]));
            REQUIRE(wmtk::simplex::utils::SimplexComparisons::equal(*m, e0, e1));


            CHECK(indices_acc.const_scalar_attribute(e0) == 1);
            CHECK(indices_acc.const_scalar_attribute(e1) == 1);

            CHECK(indices_acc.const_scalar_attribute(m->switch_vertex(e0.tuple())) == 2);
            CHECK(indices_acc.const_scalar_attribute(m->switch_vertex(e1.tuple())) == 3);
            REQUIRE(wmtk::simplex::utils::SimplexComparisons::equal(
                *m,
                wmtk::PrimitiveType::Vertex,
                m->switch_tuples(e0.tuple(), {PV, PE, PF, PE, PV}),
                m->switch_tuples(e1.tuple(), {PV, PE, PF, PE, PV})));
            CHECK(
                indices_acc.const_scalar_attribute(
                    m->switch_tuples(e0.tuple(), {PV, PE, PF, PE, PV})) == 0);
        }
    }
}
