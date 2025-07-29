#pragma once
#include <wmtk/invariants/Invariant.hpp>
#include "IsotropicRemeshingOptions.hpp"
#include "wmtk/attribute/MeshAttributeHandle.hpp"


namespace wmtk {
namespace operations {
class EdgeCollapse;
class EdgeSplit;
namespace composite {
class EdgeSwap;
}
class Operation;
class AttributesUpdateWithFunction;
} // namespace operations
namespace invariants {
class EnvelopeInvariant;
class InteriorSimplexInvariant;
class InvariantCollection;
} // namespace invariants
} // namespace wmtk
namespace wmtk::components::isotropic_remeshing {

class IsotropicRemeshing
{
public:
    IsotropicRemeshing(multimesh::MeshCollection& mc, const IsotropicRemeshingOptions& opts);
    ~IsotropicRemeshing();


    void run();
    void run(const Pass& pass, size_t pass_index);
    attribute::MeshAttributeHandle get_attribute(
        const multimesh::utils::AttributeDescription& ad) const;

private:
    std::vector<wmtk::attribute::MeshAttributeHandle> all_envelope_positions(
        const IsotropicRemeshingOptions& opts) const;
    static bool is_envelope_position(const wmtk::attribute::MeshAttributeHandle& position);
    void make_envelopes(const IsotropicRemeshingOptions& opts);
    void make_envelope_invariants(const IsotropicRemeshingOptions& opts);
    void make_interior_invariants(const IsotropicRemeshingOptions& opts);

private:
    multimesh::MeshCollection& m_meshes;

    bool start_with_collapse = false;


    size_t iterations;
    std::vector<Pass> passes;


    // format for outputting intermediate results. Assumed to just be a frame number, i.e something
    // like format("path_{}.hdf5",0) to generate path_0.hdf5
    std::vector<std::pair<std::string, wmtk::components::output::OutputOptions>>
        intermediate_output_format;
    ///---------------------

    std::shared_ptr<operations::EdgeSplit> m_split;
    std::shared_ptr<operations::EdgeCollapse> m_collapse;
    std::shared_ptr<operations::composite::EdgeSwap> m_swap;
    std::shared_ptr<operations::Operation> m_smooth;

    std::vector<std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase>>
        m_operation_transfers;

    std::map<std::string, std::shared_ptr<operations::Operation>> m_operations;
    // std::vector<std::shared_ptr<wmtk::invariants::EnvelopeInvariant>> m_envelope_invariants;
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
