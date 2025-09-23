#pragma once
#include "InvariantOptions.hpp"

namespace wmtk::components::configurator::invariants {

struct EnvelopeInvariantParameters : public AttributeInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription envelope;
    double size = 1e-3;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        EnvelopeInvariantParameters,
        attribute,
        size,
        envelope);
};
struct ThresholdInvariantParameters : public AttributeInvariantParameters
{
    double threshold;
    //    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(ThresholdInvariantParameters, attribute, threshold);
};

struct MinEdgeLengthInvariantParameters : public AttributeInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription envelope;
    double threshold;
    //    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(EnvelopeInvariantOptions)
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(
        MinEdgeLengthInvariantParameters,
        attribute,
        threshold);
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
