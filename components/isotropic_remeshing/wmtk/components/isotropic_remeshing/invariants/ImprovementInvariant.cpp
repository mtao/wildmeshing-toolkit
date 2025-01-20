
#include "ImprovementInvariant.hpp"

#include <wmtk/Mesh.hpp>
#include <wmtk/utils/Logger.hpp>

namespace wmtk::components::isotropic_remeshing::invariants {

ImprovementInvariant::ImprovementInvariant(const attribute::MeshAttributeHandle& attribute_handle)
    : Invariant(attribute_handle.mesh(), false, true, true)
    , m_attribute(attribute_handle)
{}

bool ImprovementInvariant::after(
    const std::vector<Tuple>& top_dimension_tuples_before,
    const std::vector<Tuple>& top_dimension_tuples_after) const
{

    auto op = [this](const std::vector<Tuple>& t) -> double {
        const auto acc = mesh().create_const_accessor(m_attribute.as<double>());

        double min = std::numeric_limits<double>::max();
        for(const auto& u: t) {
        min = std::min(min,acc.const_scalar_attribute(u));

        }

        return min;
    };

    const double before = mesh().parent_scope(op, top_dimension_tuples_before);
    const double after = op(top_dimension_tuples_after);


    return after >= before;
}
} // namespace wmtk::components::isotropic_remeshing::invariants
