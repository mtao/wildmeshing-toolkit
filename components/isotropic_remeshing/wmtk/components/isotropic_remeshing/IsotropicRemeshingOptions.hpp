#pragma once
#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/components/mesh_info/transfer/TransferStrategyFactoryCollection.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include "OperationOptions.hpp"


namespace wmtk::components::multimesh {
class MeshCollection;
}
namespace wmtk::components::output {
class MeshCollection;
}

namespace wmtk::components::isotropic_remeshing {


struct IsotropicRemeshingOptions
{
    IsotropicRemeshingOptions();
    // std::shared_ptr<TriMesh> input;
    wmtk::attribute::MeshAttributeHandle position_attribute;
    std::optional<wmtk::attribute::MeshAttributeHandle> inversion_position_attribute;
    std::vector<wmtk::attribute::MeshAttributeHandle> other_position_attributes;
    std::vector<
        std::pair<wmtk::attribute::MeshAttributeHandle, wmtk::attribute::MeshAttributeHandle>>
        copied_attributes;
    std::optional<wmtk::attribute::MeshAttributeHandle> sizing_field_attribute;
    std::optional<wmtk::attribute::MeshAttributeHandle> visited_edge_flag;
    std::optional<wmtk::attribute::MeshAttributeHandle> target_edge_length;

    // meshes that should not be geometrically changed
    std::vector<std::string> static_meshes;

    std::vector<wmtk::attribute::MeshAttributeHandle> pass_through_attributes;
    int64_t iterations = 10;
    double length_abs = 0;
    double length_rel = 0;
    bool lock_boundary = false;
    bool use_for_periodic = false;
    bool fix_uv_seam = false;

    bool start_with_collapse = false;

    EdgeSplitOptions split;
    EdgeCollapseOptions collapse;
    VertexSmoothOptions smooth;
    EdgeSwapOptions swap;
    mesh_info::transfer::TransferStrategyFactoryCollection utility_attributes;

    std::vector<Pass> passes;

    std::vector<attribute::MeshAttributeHandle> improvement_attributes;

    // this should be true for periodic
    bool separate_substructures = false;


    std::optional<double> envelope_size; // 1e-3


    void load_json(const nlohmann::json& js, wmtk::components::multimesh::MeshCollection& mc);
    void write_json(nlohmann::json& js) const;


    // transforms the absoltue or relative length paramters into an absolute length parameter
    double get_absolute_length() const;


    friend void to_json(
        nlohmann::json& nlohmann_json_j,
        const IsotropicRemeshingOptions& nlohmann_json_t);
    friend void from_json(
        const nlohmann::json& nlohmann_json_j,
        IsotropicRemeshingOptions& nlohmann_json_t);

    std::vector<wmtk::attribute::MeshAttributeHandle> all_positions() const;


    wmtk::components::multimesh::MeshCollection* mesh_collection = nullptr;
    // format for outputting intermediate results. Assumed to just be a frame number, i.e something
    // like format("path_{}.hdf5",0) to generate path_0.hdf5
    std::vector<std::pair<std::string, wmtk::components::output::OutputOptions>>
        intermediate_output_format;


    void fill_operation_parameters();
};


} // namespace wmtk::components::isotropic_remeshing
