#pragma once
#include <nlohmann/json.hpp>
#include "InvariantOptions.hpp"

namespace wmtk::components::configurator::invariants {


struct EnvelopeInvariantParameters : public ThresholdInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription envelope;
    NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE_WITH_DEFAULT(
        EnvelopeInvariantParameters,
        ThresholdInvariantParameters,
        envelope);
};


struct CannotMapSimplexInvariantParameters : public MeshInvariantParameters
{
    std::string mapped_mesh_path = "";
    int8_t simplex_dimension = 0;
    bool invert = false; //
    //    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        CannotMapSimplexInvariantParameters,
        mapped_mesh_path);
};


} // namespace wmtk::components::configurator::invariants
