#pragma once
#include "InvariantOptions.hpp"


/// Extra parameter types for invariants used


namespace wmtk {
class Mesh;
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::configurator::invariants {

struct AbsoluteThresholdParameters
{
    double threshold;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(AbsoluteThresholdParameters, threshold);
};

struct BoundingBoxDiagonalThresholdParameters
{
    double ratio;
    wmtk::components::multimesh::utils::AttributeDescription attribute;
    double threshold(const multimesh::MeshCollection& mc) const;
    NLOHMANN_DEFINE_TYPE_INTRUSIVE_WITH_DEFAULT(BoundingBoxDiagonalThresholdParameters, ratio);
};

template <typename T>
struct ScalarValueInvariantParameters : public AttributeInvariantParameters
{
    T value = 0;

    NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE_WITH_DEFAULT(
        ScalarValueInvariantParameters,
        AttributeInvariantParameters,
        value);
};


template <typename T>
struct ScaledComparisonAttributeInvariantParameters : public AttributeInvariantParameters
{
    wmtk::components::multimesh::utils::AttributeDescription comparison_attribute;
    T scaling = 1;

    NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE_WITH_DEFAULT(
        ScaledComparisonAttributeInvariantParameters,
        AttributeInvariantParameters,
        attribute,
        scaling);
};

struct ThresholdInvariantParameters : public AttributeInvariantParameters
{
    // the way the threshold should be computed
    enum class ThresholdType { Absolute, BoundingBoxDiagonalRelative };


    ThresholdType type;


    double threshold(const multimesh::MeshCollection& mc) const;

    // If the parameters requires an attribute name
    multimesh::utils::AttributeDescription attribute;

    nlohmann::json parameters;

    WMTK_NLOHMANN_JSON_DECLARATION(ThresholdInvariantParameters);
    // NLOHMANN_DEFINE_DERIVED_TYPE_INTRUSIVE_WITH_DEFAULT(
    //     ThresholdInvariantParameters,
    //     AttributeInvariantParameters,
    //     type,
    //     parameters);
};
NLOHMANN_JSON_SERIALIZE_ENUM(
    ThresholdInvariantParameters::ThresholdType,
    {{ThresholdInvariantParameters::ThresholdType::Absolute, "absolute"},
     {ThresholdInvariantParameters::ThresholdType::BoundingBoxDiagonalRelative, "bounding_box"}

    });

} // namespace wmtk::components::configurator::invariants
