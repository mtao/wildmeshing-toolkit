// DO NOT MODIFY, autogenerated from the /scripts directory
#include "subdart_maximal_action_to_face.hpp"
#include "SimplexDart.hpp"
#include "autogenerated_tables.hpp"
#include "local_id_table_offset.hpp"
namespace wmtk::autogen::tri_mesh {
int8_t subdart_maximal_action_to_face_action(
    int8_t dart_index,
    int8_t simplex_dimension,
    int8_t simplex_index)
{
    switch (simplex_dimension) {
    case 1: return subdart_preserving_action_1[dart_index][simplex_index];

    case 2: return SimplexDart::identity();
    default: break;
    }
    assert(false);
    return 0;
}

int8_t subdart_maximal_action_to_face_size(
    int8_t dart_index,
    int8_t simplex_dimension,
    int8_t simplex_index)
{
    switch (simplex_dimension) {
    case 1: return max_subdart_preservation_dimension_1[dart_index][simplex_index];

    case 2: return 2;
    default: break;
    }
    return 0;
}

std::array<int8_t, 2>
subdart_maximal_action_to_face(int8_t dart_index, int8_t simplex_dimension, int8_t simplex_index)
{
    switch (simplex_dimension) {
    case 1:
        return std::array<int8_t, 2>{
            {subdart_preserving_action_1[dart_index][simplex_index],
             max_subdart_preservation_dimension_1[dart_index][simplex_index]}};

    case 2: return std::array<int8_t, 2>{{SimplexDart::identity(), 2}};

    default: break;
    }
    assert(false);
    return {};
}
} // namespace wmtk::autogen::tri_mesh