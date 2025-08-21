#pragma once
#include <wmtk/components/configurator/Configurator.hpp>
#include <wmtk/invariants/Invariant.hpp>
#include "IsotropicRemeshingOptions.hpp"
#include "wmtk/attribute/MeshAttributeHandle.hpp"


namespace wmtk {
namespace operations {
class EdgeCollapse;
class EdgeSplit;
class AttributesUpdate;
namespace composite {
class EdgeSwap;
}
class Operation;
class AttributesUpdateWithFunction;
class AttributeTransferStsrategyBase;
} // namespace operations
namespace invariants {
class EnvelopeInvariant;
class InteriorSimplexInvariant;
class InvariantCollection;
} // namespace invariants
namespace components {
namespace multimesh {
class MeshCollection;
}
namespace configurator {
class Pass;
}
} // namespace components
} // namespace wmtk
  //
namespace wmtk::components::isotropic_remeshing {

class IsotropicRemeshing
{
public:
    using Pass = wmtk::components::configurator::Pass;
    IsotropicRemeshing(multimesh::MeshCollection& mc, const IsotropicRemeshingOptions& opts);
    // IsotropicRemeshing(IsotropicRemeshingOptions& opts);
    ~IsotropicRemeshing();


    void run();
    void run(Pass& pass, size_t pass_index);
    attribute::MeshAttributeHandle get_attribute(
        const multimesh::utils::AttributeDescription& ad) const;

private:
    std::vector<wmtk::attribute::MeshAttributeHandle> all_envelope_positions(
        const IsotropicRemeshingOptions& opts) const;
    static bool is_envelope_position(const wmtk::attribute::MeshAttributeHandle& position);
    void make_envelopes(const IsotropicRemeshingOptions& opts);
    void make_envelope_invariants(const IsotropicRemeshingOptions& opts);
    void make_interior_invariants(const IsotropicRemeshingOptions& opts);

    void load_shared_invariants(const IsotropicRemeshingOptions& opts);
    void load_transfers(const IsotropicRemeshingOptions& opts);

    void add_core_collapse_invariants(
        wmtk::operations::EdgeCollapse&,
        const IsotropicRemeshingOptions& opts);


private:
    components::configurator::Configurator m_configurator;
    multimesh::MeshCollection& mesh_collection();
    const multimesh::MeshCollection& mesh_collection() const;
    wmtk::components::configurator::Configurator& configurator();
    const wmtk::components::configurator::Configurator& configurator() const;

    bool start_with_collapse = false;


    size_t iterations;
    std::vector<Pass> passes;


    // format for outputting intermediate results. Assumed to just be a frame number, i.e something
    // like format("path_{}.hdf5",0) to generate path_0.hdf5
    std::vector<std::pair<std::string, wmtk::components::output::OutputOptions>>
        intermediate_output_format;
    ///---------------------

    std::shared_ptr<wmtk::operations::EdgeSplit> m_split;
    std::shared_ptr<wmtk::operations::EdgeCollapse> m_collapse;
    std::shared_ptr<wmtk::operations::composite::EdgeSwap> m_swap;
    std::shared_ptr<wmtk::operations::AttributesUpdate> m_smooth;

    std::vector<std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>>
        m_operation_transfers;

    // std::map<std::string, std::shared_ptr<operations::Operation>> m_operations;
    //  std::vector<std::shared_ptr<wmtk::invariants::EnvelopeInvariant>> m_envelope_invariants;
    std::shared_ptr<wmtk::invariants::InvariantCollection> m_envelope_invariants;

    std::shared_ptr<wmtk::invariants::InvariantCollection> m_interior_position_invariants;
    std::shared_ptr<wmtk::invariants::InvariantCollection> m_interior_edge_invariants;
    std::shared_ptr<wmtk::invariants::InvariantCollection> m_universal_invariants;

    void configure_split(const IsotropicRemeshingOptions& opts);
    void configure_collapse(const IsotropicRemeshingOptions& opts);
    void configure_swap(const IsotropicRemeshingOptions& opts);
    void configure_smooth(const IsotropicRemeshingOptions& opts);
};

} // namespace wmtk::components::isotropic_remeshing
