
#include "IsotropicRemeshing.hpp"
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include <wmtk/components/multimesh/utils/create_attribute.hpp>
#include <wmtk/components/multimesh/utils/detail/attribute_missing_error.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>
#include <wmtk/operations/attribute_update/make_cast_attribute_transfer_strategy.hpp>
#include <wmtk/simplex/utils/tuple_vector_to_homogeneous_simplex_vector.hpp>
#include "invariants/ImprovementInvariant.hpp"

// main execution tools
#include <wmtk/Scheduler.hpp>
#include <wmtk/multimesh/consolidate.hpp>
#include <wmtk/multimesh/utils/check_map_valid.hpp>


// utils for setting invariants
#include <wmtk/invariants/EnvelopeInvariant.hpp>
#include <wmtk/invariants/InteriorSimplexInvariant.hpp>
#include <wmtk/invariants/InvariantCollection.hpp>
#include <wmtk/invariants/SimplexInversionInvariant.hpp>

// meshvisitor requires knowing all the mesh types
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/multimesh/MultiMeshVisitor.hpp>

#include <wmtk/utils/Logger.hpp>

// for logging meshes
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/output/output.hpp>
#include <wmtk/components/output/utils/format.hpp>


// op types
#include <wmtk/operations/composite/EdgeSwap.hpp>
//
#include <Eigen/Geometry>


namespace wmtk::components::isotropic_remeshing {
auto IsotropicRemeshing::get_attribute(const multimesh::utils::AttributeDescription& ad) const
    -> attribute::MeshAttributeHandle
{
    return multimesh::utils::get_attribute(m_meshes, ad);
}

void IsotropicRemeshing::load_shared_invariants(const IsotropicRemeshingOptions& opts)
{
    auto position_attr = get_attribute(opts.position_attribute);

    if (!position_attr.is_valid()) {
        throw std::runtime_error("Isotropic remeshing run without a valid position attribute");
    }
    m_universal_invariants =
        std::make_shared<wmtk::invariants::InvariantCollection>(position_attr.mesh());
    for (const auto& attr : opts.improvement_attributes) {
        m_universal_invariants->add(
            std::make_shared<invariants::ImprovementInvariant>(get_attribute(attr)));
    }


    if (opts.envelope_size.has_value()) {
        make_envelopes(opts);
    }

    make_interior_invariants(opts);
}
void IsotropicRemeshing::load_transfers(const IsotropicRemeshingOptions& opts)
{
    for (const auto& [child, parent] : opts.copied_attributes) {
        auto parent_attr = get_attribute(parent);
        try {
            auto child_attr = get_attribute(child);
            m_operation_transfers.emplace_back(
                wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                    parent_attr,
                    child_attr));
        } catch (const components::multimesh::utils::detail::attribute_missing_error& e) {
            // find the parent attribute in the mesh collection to flush out its full description
            multimesh::utils::AttributeDescription child2(child.path, parent_attr);

            assert(child.compatible(child2));

            auto child_attr = multimesh::utils::create_attribute(m_meshes, child2);
            m_operation_transfers.emplace_back(
                wmtk::operations::attribute_update::make_cast_attribute_transfer_strategy(
                    parent_attr,
                    child_attr));
        }
    }
    for (const auto& transfer : opts.utility_attributes) {
        m_operation_transfers.emplace_back(transfer->create(m_meshes));
    }
}

IsotropicRemeshing::~IsotropicRemeshing() = default;
IsotropicRemeshing::IsotropicRemeshing(
    multimesh::MeshCollection& mc,
    const IsotropicRemeshingOptions& opts)
    : m_meshes(mc)
{
    passes = opts.passes;
    iterations = opts.iterations;
    start_with_collapse = opts.start_with_collapse;

    intermediate_output_format = opts.intermediate_output_format;


    load_shared_invariants(opts);
    load_transfers(opts);


    // split
    if (opts.split.enabled) {
        configure_split(opts);
        assert(bool(m_split));
        m_operations.emplace("split", m_split);
    } else {
        wmtk::logger().info("Running Isotropic Remeshing without a split configured");
    }


    //////////////////////////////////////////
    // collapse

    if (opts.collapse.enabled) {
        configure_collapse(opts);
        assert(bool(m_collapse));
        m_operations.emplace("collapse", m_collapse);
    } else {
        wmtk::logger().info("Running Isotropic Remeshing without a collapse configured");
    }


    //////////////////////////////////////////
    // swap

    if (opts.swap.enabled) {
        configure_swap(opts);
        assert(bool(m_swap));
        m_operations.emplace("swap", m_swap);
    } else if (opts.swap.mode != EdgeSwapMode::Skip) {
        wmtk::logger().info(
            "Running Isotropic Remeshing without a swap configured despite being "
            "supposed to use them");
    }

    //////////////////////////////////////////
    // smooth
    if (opts.smooth.enabled) {
        configure_smooth(opts);
        assert(bool(m_smooth));
        m_operations.emplace("smooth", m_smooth);
    } else {
        wmtk::logger().info("Running Isotropic Remeshing without a smooth configured");
    }

    if (opts.passes.empty()) {
        auto position_attr = get_attribute(opts.position_attribute);

        if (!position_attr.is_valid()) {
            throw std::runtime_error("Isotropic remeshing run without a valid position attribute");
        }
        passes.emplace_back(
            Pass{
                m_meshes.get_mesh_path(position_attr.mesh()),
                1,
                {"split", "collapse", "swap", "smooth"}});
    } else {
        passes = opts.passes;
    }
    for (size_t j = 0; j < passes.size(); ++j) {
        auto& pass = passes[j];
        if (pass.operations.empty()) {
            wmtk::logger().info(
                "Pass of isotropic remeshing didn't specify any operations, "
                "assuming it's a split,collapse,swap,smooth");
            pass.operations = {"split", "collapse", "swap", "smooth"};
        }
        for (const auto& op_name : pass.operations) {
            if (m_operations.find(op_name) == m_operations.end()) {
                throw std::runtime_error(
                    fmt::format("was unable to find operation {} in pass {}", op_name, j));
            }
        }
    }
    for (const auto& [name, op] : m_operations) {
        if (!op->attribute_new_all_configured()) {
            wmtk::log_and_throw_error("Not every attribute in {} was configured", name);
        }
    }
}

std::vector<wmtk::attribute::MeshAttributeHandle> IsotropicRemeshing::all_envelope_positions(
    const IsotropicRemeshingOptions& opts) const
{
    std::vector<wmtk::attribute::MeshAttributeHandle> handles;
    if (opts.envelope_size.has_value()) {
        auto try_add = [&](const wmtk::attribute::MeshAttributeHandle& h) {
            if (is_envelope_position(h)) {
                handles.emplace_back(h);
            }
        };

        for (const auto& h : opts.all_positions()) {
            try_add(get_attribute(h));
        }
    }
    return handles;
}

bool IsotropicRemeshing::is_envelope_position(const wmtk::attribute::MeshAttributeHandle& position)
{
    return position.mesh().top_cell_dimension() < position.dimension();
}


void IsotropicRemeshing::make_interior_invariants(const IsotropicRemeshingOptions& opts)
{
    auto position = get_attribute(opts.position_attribute);
    Mesh& mesh = position.mesh();
    auto invariant_interior_vertex = std::make_shared<wmtk::invariants::InvariantCollection>(mesh);
    m_interior_edge_invariants = std::make_shared<wmtk::invariants::InvariantCollection>(mesh);

    auto set_all_invariants = [&](auto&& m) {
        // TODO: this used to do vertex+edge, but just checkign for vertex should be sufficient?
        for (PrimitiveType pt = PrimitiveType::Vertex; pt < m.top_simplex_type(); pt = pt + 1) {
            invariant_interior_vertex->add(
                std::make_shared<wmtk::invariants::InteriorSimplexInvariant>(m, pt));
        }

        m_interior_edge_invariants->add(
            std::make_shared<wmtk::invariants::InteriorSimplexInvariant>(m, PrimitiveType::Edge));
    };
    wmtk::multimesh::MultiMeshVisitor visitor(set_all_invariants);
    visitor.execute_from_root(mesh);
    m_interior_position_invariants = invariant_interior_vertex;
}

void IsotropicRemeshing::run()
{
    wmtk::logger().info(
        "Running {} different types of passes for {} iterations",
        passes.size(),
        iterations);

    if (m_split) {
        wmtk::logger().debug(
            "Isotropic Remeshing split has invariant [{}]",
            m_split->invariants().name());
    }
    if (m_collapse) {
        wmtk::logger().debug(
            "Isotropic Remeshing collapse has invariant [{}]",
            m_collapse->invariants().name());
    }
    if (m_swap) {
        wmtk::logger().debug(
            "Isotropic Remeshing swap has invariant [{}], and its split has [{}], collapse has "
            "[{}]",
            m_swap->invariants().name(),
            m_swap->split().invariants().name(),
            m_swap->collapse().invariants().name());
    }
    if (m_smooth) {
        wmtk::logger().debug(
            "Isotropic Remeshing smooth has invariant [{}]",
            m_smooth->invariants().name());
    }

    auto log_mesh = [&](int64_t index) {
        for (const auto& [name, mesh] : m_meshes.all_meshes()) {
            spdlog::info(
                "Mesh [{}] has {} {}-facets",
                name,
                mesh.get_all(mesh.top_simplex_type()).size(),
                mesh.top_cell_dimension());
        }
        if (intermediate_output_format.empty()) {
            wmtk::logger().error(
                "Failed to log using intermediate_output_format because "
                "no MeshCollection was attached");
            return;
        }
        for (const auto& [name, opts] : intermediate_output_format) {
            auto opt = wmtk::components::output::utils::format(opts, index);
            spdlog::info("Temp logging {} as {}", name, opt.path.string());
            wmtk::components::output::output(m_meshes.get_mesh(name), opt);
        }
    };

    log_mesh(0);
    size_t index = 0;
    for (size_t k = 1; k <= iterations; ++k) {
        for (size_t j = 0; j < passes.size(); ++j) {
            Pass& p = passes[j];
            wmtk::logger().info(
                "Running pass {}/{} of iteration {}/{}. Has {} sub-iterations on {})",
                j,
                passes.size(),
                k,
                iterations,
                p.iterations,
                p.mesh_path);
            run(p, j);
        }
        log_mesh(k);
    }
}
void IsotropicRemeshing::run(const Pass& pass, size_t pass_index)
{
    // TODO: brig me back!
    // mesh_in->clear_attributes(keeps);

    // gather handles again as they were invalidated by clear_attributes
    // positions = utils::get_attributes(cache, *mesh_in,
    // opts.position_attribute); assert(positions.size() == 1); position =
    // positions.front(); pass_through_attributes = utils::get_attributes(cache,
    // *mesh_in, opts.pass_through_attributes);


    // assert(dynamic_cast<TriMesh*>(&position.mesh()) != nullptr);

    // TriMesh& mesh = static_cast<TriMesh&>(position.mesh());


    //////////////////////////////////////////
    Scheduler scheduler;
    auto& pass_mesh = m_meshes.get_mesh(pass.mesh_path);

    auto run_opo = [&](wmtk::operations::Operation& op, std::string_view op_name) {
        //auto& run_mesh = op.mesh();

        auto& run_mesh = pass_mesh;
        SchedulerStats stats = scheduler.run_operation_on_all(op, run_mesh);

        logger().info(
            "Executed {} {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, executing: {}",
            stats.number_of_performed_operations(),
            op_name,
            stats.number_of_successful_operations(),
            stats.number_of_failed_operations(),
            stats.collecting_time,
            stats.sorting_time,
            stats.executing_time);
        return stats;
    };

    // scheduler.set_update_frequency(1000);
    if (start_with_collapse) {
        if (bool(m_swap)) {
            spdlog::info("Doing an initial pass of swaps");
            run_opo(*m_swap, "initial swap");
            wmtk::multimesh::consolidate(m_swap->mesh());
        }
        if (bool(m_collapse)) {
            spdlog::info("Doing an initial pass of collapses");
            run_opo(*m_collapse, "initial collapse");
            wmtk::multimesh::consolidate(m_collapse->mesh());
        }
    }


    for (long i = 0; i < pass.iterations; ++i) {
        wmtk::logger().info("Pass {}, Sub-Iteration {}", pass_index, i);

        SchedulerStats pass_stats;
        for (const auto& name : pass.operations) {
            // for (const auto& [name, opptr] : m_operations) {
            auto& opptr = m_operations.at(name);
            if (!bool(opptr)) {
                spdlog::warn("op {} is empty", name);
                continue;
            }
            const auto stats = run_opo(*opptr, name);
            pass_stats += stats;
        }

        // TODO: find a better canonical way to do this
        wmtk::multimesh::consolidate(m_operations.begin()->second->mesh());

        logger().info(
            "Executed {} ops (S/F) {}/{}. Time: collecting: {}, sorting: {}, executing: {}",
            pass_stats.number_of_performed_operations(),
            pass_stats.number_of_successful_operations(),
            pass_stats.number_of_failed_operations(),
            pass_stats.collecting_time,
            pass_stats.sorting_time,
            pass_stats.executing_time);
    }
}

void IsotropicRemeshing::make_envelope_invariants(const IsotropicRemeshingOptions& opts)
{
    make_envelopes(opts);
}

void IsotropicRemeshing::make_envelopes(const IsotropicRemeshingOptions& opts)
{
    if (!opts.envelope_size.has_value()) {
        throw std::runtime_error(
            "Could not create envelopes because options do not have an envelope size");
    }
    auto envelope_positions = all_envelope_positions(opts);

    std::vector<std::shared_ptr<wmtk::invariants::EnvelopeInvariant>> envelope_invariants;


    std::transform(
        envelope_positions.begin(),
        envelope_positions.end(),
        std::back_inserter(envelope_invariants),
        [&](const wmtk::attribute::MeshAttributeHandle& mah) {
            spdlog::warn(
                "Envelope made for {}",
                primitive_type_name(mah.mesh().top_simplex_type()));
            return std::make_shared<wmtk::invariants::EnvelopeInvariant>(
                mah,
                std::sqrt(2) * opts.envelope_size.value(),
                mah);
        });

    m_envelope_invariants = std::make_shared<wmtk::invariants::InvariantCollection>(
        get_attribute(opts.position_attribute).mesh().get_multi_mesh_root());

    for (const auto& invar : envelope_invariants) {
        m_envelope_invariants->add(invar);
    }
}
} // namespace wmtk::components::isotropic_remeshing
