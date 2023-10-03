#pragma once
#include <catch2/catch_test_macros.hpp>
#include <wmtk/Scheduler.hpp>
#include <wmtk/function/AMIPS.hpp>
#include <wmtk/operations/OperationDifferentiableSmoothFactory.hpp>
#include <wmtk/operations/tri_mesh/VertexSmoothNewtonMethodWithLineSearch.hpp>
#include <wmtk/operations/tri_mesh/VertexSmoothUsingDifferentiableEnergy.hpp>
#include <wmtk/utils/Logger.hpp>
#include "../tools/DEBUG_TriMesh.hpp"
#include "../tools/TriMesh_examples.hpp"
using namespace wmtk;
using namespace wmtk::tests;
using namespace wmtk::operations;

TEST_CASE("smoothing_using_differentiable_energy")
{
    DEBUG_TriMesh mesh = ten_triangles_with_position(2);
    OperationSettings<tri_mesh::VertexSmoothUsingDifferentiableEnergy> op_settings;
    op_settings.uv_position = mesh.get_attribute_handle<double>("position", PrimitiveType::Vertex);
    op_settings.smooth_boundary = false;
    op_settings.second_order = true;
    op_settings.line_search = false;
    op_settings.energy = std::make_unique<function::AMIPS_2D>(
        mesh,
        mesh.get_attribute_handle<double>("position", PrimitiveType::Vertex));
    op_settings.initialize_invariants(mesh);
    Scheduler scheduler(mesh);
    scheduler.add_operation_factory(
        "tri_mesh_smooth_vertex_newton_method",
        std::make_unique<operations::OperationDifferentiableSmoothFactory>(op_settings));
    scheduler.run_operation_on_all(PrimitiveType::Vertex, "tri_mesh_smooth_vertex_newton_method");
    ConstAccessor<double> pos = mesh.create_const_accessor(op_settings.uv_position);
    Tuple tuple = mesh.face_tuple_from_vids(2, 4, 5);
    Eigen::Vector2d uv0 = pos.const_vector_attribute(tuple);
    Eigen::Vector2d uv1 = pos.const_vector_attribute(mesh.switch_vertex(tuple));
    Eigen::Vector2d uv2 = pos.const_vector_attribute(mesh.switch_vertex(mesh.switch_edge(tuple)));
    wmtk::logger().info("uv0 {}", uv0.transpose());
    wmtk::logger().info("uv1 {}", uv1.transpose());
    wmtk::logger().info("uv2 {}", uv2.transpose());
}
