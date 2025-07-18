#pragma once
#include <array>
#include <wmtk/dart/Dart.hpp>
#include <wmtk/dart/SimplexDart.hpp>
namespace wmtk {
class Mesh;
namespace autogen {
class SimplexDart;
}
} // namespace wmtk
namespace wmtk::operations::internal {
class SplitAlternateFacetOptionData
{
public:
    using Dart = dart::Dart;
    SplitAlternateFacetOptionData(
        const Dart& input_tuple,
        const std::array<int64_t, 2>& new_facet_indices);

    Dart input;

    // Stores {ear_global_id, M}
    // where M is defined by:
    // Let {G, O} be the input dart
    // Let {Ge, Oe} be the left/0 or right/1 ear opposite
    // Let R be such that Oe = R O
    // We select an arbitrary dart that includes the right face
    // {G,Oa} a dart that includes the ear face, and
    // {Ge,Ob} a dart denoting a art on Ge whose D-1 subdart is equivalent
    // Let M be  Ob = M Oa.
    // Note that for D-1 subdart encoded on G, M will return the equivalent D-1 subdart on Ge
    //
    std::array<int64_t, 2> new_facet_indices;

    // Let d be a dart where every D-simplex for D <the input mesh dimension
    // lies in left/index=0 (equivalently right/index=1) ear then
    // returns a dart such that those simplices are preserved using the other face
    //
    // This is given by the definition of alts and applying
    // Let {G, Od} be d
    // We compute {G, M Od}
    //
    int64_t new_gid(PrimitiveType primitive_type, int8_t index) const;


private:
    // returns the indices of the ear boundary simplices
    const std::array<int8_t, 2>& boundary_indices(PrimitiveType pt) const;
};

inline bool operator==(const SplitAlternateFacetOptionData& a, int64_t b)
{
    return a.input.global_id() == b;
}
} // namespace wmtk::operations::internal
