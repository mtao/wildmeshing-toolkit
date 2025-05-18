#include "from_facet_orientations.hpp"
#include <numeric>
#include <wmtk/Mesh.hpp>
#include <wmtk/utils/internal/identity_tuple.hpp>


namespace wmtk::multimesh {

std::vector<std::array<Tuple, 2>> from_facet_orientations(
    const Mesh& parent,
    const Mesh& child,
    const std::vector<Tuple>& parent_tuples)
{
    std::vector<std::array<Tuple, 2>> ret(parent_tuples.size());
    PrimitiveType pt = child.top_simplex_type();
    assert(parent_tuples.size() == child.capacity(pt));

#if !defined(WMTK_UNIFY_DART_AND_TUPLES)
    const auto child_top_dimension_tuples = child.get_all(pt);
#endif

    for (size_t i = 0; i < parent_tuples.size(); ++i) {
#if defined(WMTK_UNIFY_DART_AND_TUPLES)
        const Tuple t = wmtk::utils::internal::identity_tuple(pt,i);
#else
        const Tuple t = wmtk::utils::internal::identity_tuple(pt,i);
        //const Tuple t = child_top_dimension_tuples[i];
#endif
        ret[i] = {{t, parent_tuples[i]}};
    }
    return ret;
}
} // namespace wmtk::multimesh
