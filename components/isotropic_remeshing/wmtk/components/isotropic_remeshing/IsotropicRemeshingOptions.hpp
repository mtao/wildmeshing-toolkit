#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/multimesh/utils/AttributeDescription.hpp>
#include "OperationOptions.hpp"
#include <wmtk/components/mesh_info/transfer/TransferStrategyFactoryCollection.hpp>
#include <wmtk/components/output/OutputOptions.hpp>

namespace wmtk::components::isotropic_remeshing {


struct IsotropicRemeshingOptions
{
    using AttributeDescription = components::multimesh::utils::AttributeDescription;
    IsotropicRemeshingOptions();


    AttributeDescription position_attribute;
    //AttributeDescription inversion_position_attribute;
    //AttributeDescription other_position_attributes;

    // attributes that will just be copied from some other attribute; deprecated by utility_attributes
    std::vector<
        std::pair<AttributeDescription, AttributeDescription>>
        copied_attributes;
    // attributes that we will ignore transferring; deprecated because this isn't useful
    std::vector<AttributeDescription> pass_through_attributes;

    //AttributeDescription sizing_field_attribute;
    //AttributeDescription visited_edge_flag;
    //AttributeDescription target_edge_length;

    // meshes that should not be geometrically changed by operations of dimension higher than it
    std::vector<std::string> static_meshes;

    mesh_info::transfer::TransferStrategyFactoryCollection utility_attributes;

    // attributes that need to be improved for an operation to be accepted
    std::vector<AttributeDescription> improvement_attributes;

    
    std::vector<Pass> passes;
    int64_t iterations = 10;


    double length_abs = 0;
    double length_rel = 0;

    bool start_with_collapse = false;

    EdgeSplitOptions split;
    EdgeCollapseOptions collapse;
    VertexSmoothOptions smooth;
    EdgeSwapOptions swap;



    std::optional<double> envelope_size; // 1e-3



    // transforms the absoltue or relative length paramters into an absolute length parameter
    double get_absolute_length(const multimesh::MeshCollection& mesh) const;


    friend void to_json(
        nlohmann::json& nlohmann_json_j,
        const IsotropicRemeshingOptions& nlohmann_json_t);
    friend void from_json(
        const nlohmann::json& nlohmann_json_j,
        IsotropicRemeshingOptions& nlohmann_json_t);

    std::vector<AttributeDescription> all_positions() const;


    // format for outputting intermediate results. Assumed to just be a frame number, i.e something
    // like format("path_{}.hdf5",0) to generate path_0.hdf5
    std::vector<std::pair<std::string, wmtk::components::output::OutputOptions>>
        intermediate_output_format;


};


} // namespace wmtk::components::isotropic_remeshing
