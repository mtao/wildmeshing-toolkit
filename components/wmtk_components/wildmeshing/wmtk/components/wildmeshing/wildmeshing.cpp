#include "wildmeshing.hpp"

#include "WildmeshingOptions.hpp"

#include <wmtk/Mesh.hpp>
#include <wmtk/Scheduler.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>

#include <wmtk/components/base/get_attributes.hpp>
#include <wmtk/utils/Logger.hpp>

#include <wmtk/operations/attribute_new/SplitNewAttributeStrategy.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>

#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/operations/OptimizationSmoothing.hpp>
#include <wmtk/operations/composite/ProjectOperation.hpp>
#include <wmtk/operations/composite/TetEdgeSwap.hpp>
#include <wmtk/operations/composite/TetFaceSwap.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>


#include <wmtk/function/LocalNeighborsSumFunction.hpp>
#include <wmtk/function/PerSimplexFunction.hpp>
#include <wmtk/function/simplex/AMIPS.hpp>

#include <wmtk/invariants/EdgeValenceInvariant.hpp>
#include <wmtk/invariants/EnvelopeInvariant.hpp>
#include <wmtk/invariants/FunctionInvariant.hpp>
#include <wmtk/invariants/InteriorEdgeInvariant.hpp>
#include <wmtk/invariants/InteriorSimplexInvariant.hpp>
#include <wmtk/invariants/InteriorVertexInvariant.hpp>
#include <wmtk/invariants/MaxFunctionInvariant.hpp>
#include <wmtk/invariants/MultiMeshLinkConditionInvariant.hpp>
#include <wmtk/invariants/NoBoundaryCollapseToInteriorInvariant.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>
#include <wmtk/invariants/TodoInvariant.hpp>

#include <wmtk/multimesh/utils/extract_child_mesh_from_tag.hpp>

#include <wmtk/io/MeshReader.hpp>
#include <wmtk/io/ParaviewWriter.hpp>


namespace wmtk::components {

using namespace simplex;
using namespace operations;
using namespace operations::tri_mesh;
using namespace operations::tet_mesh;
using namespace operations::composite;
using namespace function;
using namespace invariants;

namespace {
void write(
    const std::shared_ptr<Mesh>& mesh,
    const std::string& out_dir,
    const std::string& name,
    const std::string& vname,
    const int64_t index,
    const bool intermediate_output)
{
    if (intermediate_output) {
        if (mesh->top_simplex_type() == PrimitiveType::Face) {
            // write trimesh
            const std::filesystem::path data_dir = "";
            wmtk::io::ParaviewWriter writer(
                data_dir / (name + "_" + std::to_string(index)),
                "vertices",
                *mesh,
                true,
                true,
                true,
                false);
            mesh->serialize(writer);
        } else if (mesh->top_simplex_type() == PrimitiveType::Tetrahedron) {
            // write tetmesh
            const std::filesystem::path data_dir = "";
            wmtk::io::ParaviewWriter writer(
                data_dir / (name + "_" + std::to_string(index)),
                "vertices",
                *mesh,
                true,
                true,
                true,
                true);
            mesh->serialize(writer);
        } else if (mesh->top_simplex_type() == PrimitiveType::Edge) {
            // write edgemesh
            const std::filesystem::path data_dir = "";
            wmtk::io::ParaviewWriter writer(
                data_dir / (name + "_" + std::to_string(index)),
                "vertices",
                *mesh,
                true,
                true,
                false,
                false);
            mesh->serialize(writer);
        }
    }
}

// std::shared_ptr<operations::CollapseNewAttributeStrategy<int64_t>>
// keep_tag_strategy(Mesh& m, const attribute::MeshAttributeHandle& attr, int64_t value)
// {
//     auto strat = std::make_shared<operations::CollapseNewAttributeStrategy<int64_t>>(attr);
//     strat->set_simplex_predicate([&m, attr, value](const simplex::Simplex& s) -> bool {
//         auto acc = m.create_accessor<int64_t>(attr);
//         return acc.const_scalar_attribute(s.tuple()) != value;
//     });
//     strat->set_strategy(operations::CollapseBasicStrategy::Default);

//     return strat;
// }

} // namespace

void wildmeshing(const base::Paths& paths, const nlohmann::json& j, io::Cache& cache)
{
    //////////////////////////////////
    // Load mesh from settings
    WildmeshingOptions options = j.get<WildmeshingOptions>();
    const std::filesystem::path& file = options.input;
    auto mesh = cache.read_mesh(options.input);

    //////////////////////////////////
    // Storing edge lengths
    auto edge_length_attribute =
        mesh->register_attribute<double>("edge_length", PrimitiveType::Edge, 1);
    auto edge_length_accessor = mesh->create_accessor(edge_length_attribute.as<double>());

    //////////////////////////////////
    // auto edge_boundary_attribute =
    //     mesh->register_attribute<int64_t>("edge_boundary", PrimitiveType::Edge, 1);
    // auto edge_boundary_accessor = mesh->create_accessor(edge_boundary_attribute.as<int64_t>());


    //////////////////////////////////
    // Retriving vertices
    auto pt_attribute =
        mesh->get_attribute_handle<double>(options.attributes.position, PrimitiveType::Vertex);
    auto pt_accessor = mesh->create_accessor(pt_attribute.as<double>());

    // Edge length update
    auto compute_edge_length = [](const Eigen::MatrixXd& P) -> Eigen::VectorXd {
        assert(P.cols() == 2);
        assert(P.rows() == 2 || P.rows() == 3);
        return Eigen::VectorXd::Constant(1, (P.col(0) - P.col(1)).norm());
    };
    auto edge_length_update =
        std::make_shared<wmtk::operations::SingleAttributeTransferStrategy<double, double>>(
            edge_length_attribute,
            pt_attribute,
            compute_edge_length);

    //////////////////////////////////
    // computing edge lengths
    const auto edges = mesh->get_all(PrimitiveType::Edge);
    for (const auto& e : edges) {
        const auto p0 = pt_accessor.vector_attribute(e);
        const auto p1 = pt_accessor.vector_attribute(mesh->switch_vertex(e));

        edge_length_accessor.scalar_attribute(e) = (p0 - p1).norm();
        // edge_boundary_accessor.scalar_attribute(e) = mesh->is_boundary(Simplex::edge(e)) ? 1 : 0;
    }


    //////////////////////////////////
    // computng bbox diagonal
    Eigen::VectorXd bmin(mesh->top_cell_dimension());
    bmin.setConstant(std::numeric_limits<double>::max());
    Eigen::VectorXd bmax(mesh->top_cell_dimension());
    bmax.setConstant(std::numeric_limits<double>::min());

    const auto vertices = mesh->get_all(PrimitiveType::Vertex);
    for (const auto& v : vertices) {
        const auto p = pt_accessor.vector_attribute(v);
        for (int64_t d = 0; d < bmax.size(); ++d) {
            bmin[d] = std::min(bmin[d], p[d]);
            bmax[d] = std::max(bmax[d], p[d]);
        }
    }

    const double bbdiag = (bmax - bmin).norm();
    const double target_edge_length = options.target_edge_length * bbdiag;
    const double envelope = 1e-3 * bbdiag;

    //////////////////////////////////
    // default transfer
    auto pass_through_attributes = base::get_attributes(cache, *mesh, options.pass_through);
    pass_through_attributes.push_back(edge_length_attribute);

    //////////////////////////////////
    // Lambdas for priority
    auto long_edges_first = [&](const simplex::Simplex& s) {
        assert(s.primitive_type() == PrimitiveType::Edge);
        return std::vector<double>({-edge_length_accessor.scalar_attribute(s.tuple())});
    };
    auto short_edges_first = [&](const simplex::Simplex& s) {
        assert(s.primitive_type() == PrimitiveType::Edge);
        return std::vector<double>({edge_length_accessor.scalar_attribute(s.tuple())});
    };

    //////////////////////////////////
    // Energy to optimize
    std::shared_ptr<function::PerSimplexFunction> amips =
        std::make_shared<AMIPS>(*mesh, pt_attribute);


    opt_logger().set_level(spdlog::level::level_enum::critical);
    // logger().set_level(spdlog::level::level_enum::debug);

    //////////////////////////////////
    // is_boundary tag for envelope and boundary child mesh

    const PrimitiveType boundary_pt = get_primitive_type_from_id(mesh->top_cell_dimension() - 1);

    auto is_boundary_handle = mesh->register_attribute<int64_t>("is_boundary", boundary_pt, 1);
    auto is_boundary_accessor = mesh->create_accessor(is_boundary_handle.as<int64_t>());

    for (const auto& t : mesh->get_all(boundary_pt)) {
        is_boundary_accessor.scalar_attribute(t) = mesh->is_boundary(boundary_pt, t) ? 1 : 0;
    }

    // pass through this attribute. this is not used anymore
    pass_through_attributes.push_back(is_boundary_handle);

    // extract envelope mesh, set coordinates attribute to it
    auto envelope_mesh_ptr = wmtk::multimesh::utils::extract_and_register_child_mesh_from_tag(
        *mesh,
        "is_boundary",
        1,
        boundary_pt);

    auto envelope_position_handle = envelope_mesh_ptr->register_attribute<double>(
        "vertices",
        PrimitiveType::Vertex,
        mesh->top_cell_dimension());
    auto envelope_position_accessor =
        envelope_mesh_ptr->create_accessor(envelope_position_handle.as<double>());

    for (const auto& t : envelope_mesh_ptr->get_all(PrimitiveType::Vertex)) {
        const auto& parent_tuple =
            envelope_mesh_ptr->map_to_parent_tuple(simplex::Simplex::vertex(t));
        envelope_position_accessor.vector_attribute(t) = pt_accessor.vector_attribute(parent_tuple);
    }

    // tmp pass through position of envelope mesh
    // pass_through_attributes.push_back(envelope_position_handle);

    // child mesh postion update

    auto propagate_position = [](const Eigen::MatrixXd& P) -> Eigen::VectorXd { return P; };

    auto envelope_position_update =
        std::make_shared<wmtk::operations::SingleAttributeTransferStrategy<double, double>>(
            envelope_position_handle,
            pt_attribute,
            propagate_position);

    auto reverse_propagate_position = [](const Eigen::MatrixXd& P) -> Eigen::VectorXd {
        return P.col(0);
    };

    auto projection_position_update =
        std::make_shared<wmtk::operations::SingleAttributeTransferStrategy<double, double>>(
            pt_attribute,
            envelope_position_handle,
            reverse_propagate_position);

    //////////////////////////////////
    // Invariants
    auto envelope_invariant = std::make_shared<EnvelopeInvariant>(
        envelope_position_handle,
        1e-3,
        envelope_position_handle);

    auto inversion_invariant =
        std::make_shared<SimplexInversionInvariant>(*mesh, pt_attribute.as<double>());

    // auto function_invariant = std::make_shared<FunctionInvariant>(mesh->top_simplex_type(),
    // amips);
    auto function_invariant =
        std::make_shared<MaxFunctionInvariant>(mesh->top_simplex_type(), amips);

    //////////////////////////////////
    // Creation of the 4 ops
    std::vector<std::shared_ptr<Operation>> ops;
    std::vector<std::string> ops_name;


    // 1) EdgeSplit
    auto split = std::make_shared<EdgeSplit>(*mesh);
    split->add_invariant(std::make_shared<TodoLargerInvariant>(
        *mesh,
        edge_length_attribute.as<double>(),
        4.0 / 3.0 * target_edge_length));
    split->set_priority(long_edges_first);

    split->set_new_attribute_strategy(pt_attribute);
    split->set_new_attribute_strategy(envelope_position_handle);

    split->add_transfer_strategy(edge_length_update);
    split->add_transfer_strategy(envelope_position_update);

    for (const auto& attr : pass_through_attributes) {
        split->set_new_attribute_strategy(attr);
    }

    ops.emplace_back(split);
    ops_name.emplace_back("split");


    // 2) EdgeCollapse
    auto collapse = std::make_shared<EdgeCollapse>(*mesh);
    collapse->add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*mesh));

    // collapse->add_invariant(
    //     std::make_shared<SimplexInversionInvariant>(*mesh, pt_attribute.as<double>()));
    // collapse->add_invariant(std::make_shared<FunctionInvariant>(mesh->top_simplex_type(),
    // amips));
    // collapse->add_invariant(
    //     std::make_shared<MaxFunctionInvariant>(mesh->top_simplex_type(), amips));
    // collapse->add_invariant(std::make_shared<TodoSmallerInvariant>(
    //     *mesh,
    //     edge_length_attribute.as<double>(),
    //     4.0 / 5.0 * target_edge_length));
    // collapse->set_priority(short_edges_first);

    auto clps_strat = std::make_shared<CollapseNewAttributeStrategy<double>>(pt_attribute);
    clps_strat->set_simplex_predicate(BasicSimplexPredicate::IsInterior);
    clps_strat->set_strategy(CollapseBasicStrategy::Default);
    collapse->set_new_attribute_strategy(pt_attribute, clps_strat);
    collapse->set_new_attribute_strategy(envelope_position_handle);

    collapse->add_invariant(inversion_invariant);
    for (const auto& attr : pass_through_attributes) {
        collapse->set_new_attribute_strategy(attr);
    }
    collapse->add_transfer_strategy(envelope_position_update);
    auto proj_collapse = std::make_shared<ProjectOperation>(
        collapse,
        envelope_position_handle,
        *mesh,
        envelope_position_handle);
    proj_collapse->add_invariant(envelope_invariant);
    proj_collapse->add_invariant(inversion_invariant);
    proj_collapse->add_invariant(function_invariant);
    proj_collapse->add_invariant(std::make_shared<TodoSmallerInvariant>(
        *mesh,
        edge_length_attribute.as<double>(),
        4.0 / 5.0 * target_edge_length));
    proj_collapse->set_priority(short_edges_first);
    proj_collapse->add_transfer_strategy(edge_length_update);

    proj_collapse->add_transfer_strategy(projection_position_update);

    ops.emplace_back(proj_collapse);
    ops_name.emplace_back("collapse");


    // 3) TriEdgeSwap
    if (mesh->top_simplex_type() == PrimitiveType::Face) {
        auto swap = std::make_shared<TriEdgeSwap>(*mesh);
        swap->collapse().add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*mesh));
        swap->add_invariant(std::make_shared<InteriorEdgeInvariant>(*mesh));
        swap->add_invariant(inversion_invariant);
        swap->add_invariant(function_invariant);
        swap->set_priority(long_edges_first);

        swap->split().set_new_attribute_strategy(pt_attribute);

        swap->collapse().set_new_attribute_strategy(pt_attribute, CollapseBasicStrategy::CopyOther);

        swap->split().set_new_attribute_strategy(envelope_position_handle);
        swap->collapse().set_new_attribute_strategy(envelope_position_handle);

        swap->add_transfer_strategy(edge_length_update);


        for (const auto& attr : pass_through_attributes) {
            swap->split().set_new_attribute_strategy(attr);
            swap->collapse().set_new_attribute_strategy(attr);
        }

        swap->add_transfer_strategy(edge_length_update);
        swap->add_transfer_strategy(envelope_position_update);
        ops.push_back(swap);
        ops_name.push_back("swap");

    } else if (mesh->top_simplex_type() == PrimitiveType::Tetrahedron) {
        // 3 - 1 - 1) TetEdgeSwap 4-4 1
        auto swap44 = std::make_shared<TetEdgeSwap>(*mesh, 0);
        swap44->collapse().add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*mesh));
        swap44->add_invariant(std::make_shared<InteriorEdgeInvariant>(*mesh));
        swap44->add_invariant(
            std::make_shared<EdgeValenceInvariant>(*mesh, 4)); // extra edge valance invariant
        swap44->add_invariant(inversion_invariant);
        swap44->add_invariant(std::make_shared<FunctionInvariant>(mesh->top_simplex_type(), amips));
        swap44->set_priority(long_edges_first);

        swap44->collapse().set_new_attribute_strategy(edge_length_attribute);
        swap44->split().set_new_attribute_strategy(edge_length_attribute);

        swap44->split().set_new_attribute_strategy(pt_attribute);
        swap44->collapse().set_new_attribute_strategy(
            pt_attribute,
            CollapseBasicStrategy::CopyOther);

        swap44->split().set_new_attribute_strategy(envelope_position_handle);
        swap44->collapse().set_new_attribute_strategy(envelope_position_handle);

        swap44->split().add_transfer_strategy(envelope_position_update);
        swap44->collapse().add_transfer_strategy(envelope_position_update);

        for (const auto& attr : pass_through_attributes) {
            swap44->split().set_new_attribute_strategy(attr);
            swap44->collapse().set_new_attribute_strategy(attr);
        }

        swap44->add_transfer_strategy(edge_length_update);
        swap44->add_transfer_strategy(envelope_position_update);

        ops.push_back(swap44);
        ops_name.push_back("swap44");

        // 3 - 1 - 2) TetEdgeSwap 4-4 2
        auto swap44_2 = std::make_shared<TetEdgeSwap>(*mesh, 1);
        swap44_2->collapse().add_invariant(
            std::make_shared<MultiMeshLinkConditionInvariant>(*mesh));
        swap44_2->add_invariant(std::make_shared<InteriorEdgeInvariant>(*mesh));

        swap44_2->add_invariant(
            std::make_shared<EdgeValenceInvariant>(*mesh, 4)); // extra edge valance invariant
        swap44_2->add_invariant(inversion_invariant);
        swap44_2->add_invariant(
            std::make_shared<FunctionInvariant>(mesh->top_simplex_type(), amips));
        swap44_2->set_priority(long_edges_first);

        swap44_2->collapse().set_new_attribute_strategy(edge_length_attribute);
        swap44_2->split().set_new_attribute_strategy(edge_length_attribute);

        swap44_2->split().set_new_attribute_strategy(pt_attribute);
        swap44_2->collapse().set_new_attribute_strategy(
            pt_attribute,
            CollapseBasicStrategy::CopyOther);

        swap44_2->split().set_new_attribute_strategy(envelope_position_handle);
        swap44_2->collapse().set_new_attribute_strategy(envelope_position_handle);

        swap44_2->split().add_transfer_strategy(envelope_position_update);
        swap44_2->collapse().add_transfer_strategy(envelope_position_update);


        for (const auto& attr : pass_through_attributes) {
            swap44_2->split().set_new_attribute_strategy(attr);
            swap44_2->collapse().set_new_attribute_strategy(attr);
        }

        swap44_2->add_transfer_strategy(edge_length_update);
        swap44_2->add_transfer_strategy(envelope_position_update);

        ops.push_back(swap44_2);
        ops_name.push_back("swap44_2");

        // 3 - 2) TetEdgeSwap 3-2
        auto swap32 = std::make_shared<TetEdgeSwap>(*mesh, 0);
        swap32->collapse().add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*mesh));
        swap32->add_invariant(std::make_shared<InteriorEdgeInvariant>(*mesh));
        swap32->add_invariant(
            std::make_shared<EdgeValenceInvariant>(*mesh, 3)); // extra edge valance invariant
        swap32->add_invariant(inversion_invariant);
        // swap32->add_invariant(std::make_shared<FunctionInvariant>(mesh->top_simplex_type(),
        // amips));
        swap32->add_invariant(
            std::make_shared<MaxFunctionInvariant>(mesh->top_simplex_type(), amips));
        swap32->set_priority(long_edges_first);

        swap32->split().set_new_attribute_strategy(pt_attribute);
        swap32->collapse().set_new_attribute_strategy(
            pt_attribute,
            CollapseBasicStrategy::CopyOther);

        swap32->split().set_new_attribute_strategy(envelope_position_handle);
        swap32->collapse().set_new_attribute_strategy(envelope_position_handle);

        swap32->split().add_transfer_strategy(envelope_position_update);
        swap32->collapse().add_transfer_strategy(envelope_position_update);

        for (const auto& attr : pass_through_attributes) {
            swap32->split().set_new_attribute_strategy(attr);
            swap32->collapse().set_new_attribute_strategy(attr);
        }

        swap32->add_transfer_strategy(edge_length_update);
        swap32->add_transfer_strategy(envelope_position_update);


        ops.push_back(swap32);
        ops_name.push_back("swap32");

        // 3 - 3) TetFaceSwap 2-3

        auto swap23 = std::make_shared<TetFaceSwap>(*mesh);
        swap23->collapse().add_invariant(std::make_shared<MultiMeshLinkConditionInvariant>(*mesh));
        swap23->add_invariant(
            std::make_shared<InteriorSimplexInvariant>(*mesh, PrimitiveType::Face));
        swap23->add_invariant(inversion_invariant);
        // swap23->add_invariant(std::make_shared<FunctionInvariant>(mesh->top_simplex_type(),
        // amips));
        swap32->add_invariant(
            std::make_shared<MaxFunctionInvariant>(mesh->top_simplex_type(), amips));

        swap23->split().set_new_attribute_strategy(pt_attribute);
        swap23->collapse().set_new_attribute_strategy(pt_attribute, CollapseBasicStrategy::None);

        swap23->split().set_new_attribute_strategy(envelope_position_handle);
        swap23->collapse().set_new_attribute_strategy(envelope_position_handle);

        swap23->split().add_transfer_strategy(envelope_position_update);
        swap23->collapse().add_transfer_strategy(envelope_position_update);

        for (const auto& attr : pass_through_attributes) {
            swap23->split().set_new_attribute_strategy(attr);
            swap23->collapse().set_new_attribute_strategy(attr);
        }

        swap23->add_transfer_strategy(edge_length_update);
        swap23->add_transfer_strategy(envelope_position_update);

        ops.push_back(swap23);
        ops_name.push_back("swap23");
    }

    // 4) Smoothing
    auto energy =
        std::make_shared<function::LocalNeighborsSumFunction>(*mesh, pt_attribute, *amips);
    auto smoothing = std::make_shared<OptimizationSmoothing>(energy);
    smoothing->add_invariant(inversion_invariant);
    smoothing->add_transfer_strategy(envelope_position_update);
    auto proj_smoothing = std::make_shared<ProjectOperation>(
        smoothing,
        envelope_position_handle,
        *mesh,
        envelope_position_handle);
    proj_smoothing->add_invariant(envelope_invariant);
    proj_smoothing->add_transfer_strategy(edge_length_update);

    // proj_smoothing->add_transfer_strategy(envelope_position_update);

    proj_smoothing->add_transfer_strategy(projection_position_update);
    proj_smoothing->use_random_priority() = false;
    proj_smoothing->add_invariant(inversion_invariant);
    ops.push_back(proj_smoothing);

    // 29 parent 3 child


    // smoothing->add_invariant(envelope_invariant);
    // smoothing->add_transfer_strategy(edge_length_update);
    // smoothing->use_random_priority() = true;
    // ops.push_back(smoothing);

    ops_name.push_back("smoothing");

    write(
        mesh,
        paths.output_dir,
        options.output,
        options.attributes.position,
        0,
        options.intermediate_output);

    write(
        envelope_mesh_ptr,
        paths.output_dir,
        options.output + "_boundary",
        options.attributes.position,
        0,
        options.intermediate_output);


    //////////////////////////////////
    // Running all ops in order n times
    Scheduler scheduler;
    int iii = 0;
    for (int64_t i = 0; i < options.passes; ++i) {
        logger().info("Pass {}", i);
        SchedulerStats pass_stats;
        int jj = 0;
        for (auto& op : ops) {
            auto stats = scheduler.run_operation_on_all(*op);
            pass_stats += stats;
            logger().info(
                "Executed {}, {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, "
                "executing: {}",
                ops_name[jj],
                stats.number_of_performed_operations(),
                stats.number_of_successful_operations(),
                stats.number_of_failed_operations(),
                stats.collecting_time,
                stats.sorting_time,
                stats.executing_time);
            ++jj;

            write(
                mesh,
                paths.output_dir,
                options.output,
                options.attributes.position,
                iii + 1,
                options.intermediate_output);


            write(
                envelope_mesh_ptr,
                paths.output_dir,
                options.output + "_boundary",
                options.attributes.position,
                iii + 1,
                options.intermediate_output);
            ++iii;
        }

        logger().info(
            "Executed {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, executing: {}",
            pass_stats.number_of_performed_operations(),
            pass_stats.number_of_successful_operations(),
            pass_stats.number_of_failed_operations(),
            pass_stats.collecting_time,
            pass_stats.sorting_time,
            pass_stats.executing_time);

        // write(
        //     mesh,
        //     paths.output_dir,
        //     options.output,
        //     options.attributes.position,
        //     i + 1,
        //     options.intermediate_output);

        assert(mesh->is_connectivity_valid());

        // write(
        //     envelope_mesh_ptr,
        //     paths.output_dir,
        //     options.output + "_boundary",
        //     options.attributes.position,
        //     i + 1,
        //     options.intermediate_output);
    }
}
} // namespace wmtk::components
