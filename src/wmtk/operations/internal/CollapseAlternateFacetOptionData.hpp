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
class CollapseAlternateFacetOptionData
{
public:
    using Dart = dart::Dart;
    CollapseAlternateFacetOptionData(const Mesh& m, const Tuple& input_tuple);
    CollapseAlternateFacetOptionData(
        const Mesh& m,
        const dart::SimplexDart& sd,
        const Tuple& input_tuple);
    dart::Dart input;

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
    std::array<dart::Dart, 2> alts;

    // Let d be a dart where every D-simplex for D <the input mesh dimension
    // lies in left/index=0 (equivalently right/index=1) ear then
    // returns a dart such that those simplices are preserved using the other face
    //
    // This is given by the definition of alts and applying
    // Let {G, Od} be d
    // We compute {G, M Od}
    //
    Dart convert(const Dart& d, size_t index) const;

    // index is which ear we're currently at
    Dart map_permutation_to_alt(const wmtk::dart::SimplexDart& sd, int8_t permutation, int8_t index)
        const;
    // const Dart& best_alt(const wmtk::dart::SimplexDart& sd, int8_t permutation) const;
    std::array<int8_t, 2> largest_subdarts(const wmtk::dart::SimplexDart& sd, int8_t permutation)
        const;


    int8_t largest_subdart(const wmtk::dart::SimplexDart& sd, int8_t permutation, int8_t index)
        const;
    std::array<int8_t, 2> local_boundary_indices(const wmtk::dart::SimplexDart& sd) const;
    int8_t local_boundary_index(const wmtk::dart::SimplexDart& sd, int8_t index) const;

    Dart map_dart_to_alt(const wmtk::dart::SimplexDart& sd, const Dart& d) const;

    // selects which index to use // false = 0, true = 1
    Dart map_permutation_to_alt(const wmtk::dart::SimplexDart& sd, int8_t permutation) const;


    static auto get_left_face(const Mesh& m, const Tuple& t) -> Tuple;
    static auto get_right_face(const Mesh& m, const Tuple& t) -> Tuple;
    // static Dart::

private:
    static auto get_face_from_action(const Mesh& m, const Tuple& t, int8_t local_action) -> Tuple;
    Dart left_switches(const Mesh& m, const Tuple& t) const;
    Dart right_switches(const Mesh& m, const Tuple& t) const;
    // given an ear tuple reports the relative orientation across the edge
    Dart get_action(const Mesh& m, const Tuple& t) const;
};
inline bool operator==(const CollapseAlternateFacetOptionData& a, int64_t b)
{
    return a.input.global_id() == b;
}
} // namespace wmtk::operations::internal
