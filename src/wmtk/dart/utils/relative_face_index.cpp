#include "relative_face_index.hpp"
#include <wmtk/dart/SimplexDart.hpp>
//#include "get_canonical_simplex_orientation.hpp"

namespace wmtk::autogen {
class SimplexDart;
}
namespace wmtk::dart::utils {
int8_t relative_face_index(
    const SimplexDart& sd,
    int8_t current_orientation,
    PrimitiveType target_type,
    int8_t target_index)
{
    //const int8_t identity_action_to_right_face = get_canonical_simplex_orientation(sd, target_type, sd.simplex_index(current_orientation, target_type)); 
    //const int8_t action_to_right_face =
    //    sd.product(identity_action_to_right_face, sd.inverse(current_orientation));

    const int8_t relative_face = sd.simplex_index(sd.inverse(current_orientation), target_type);
    return relative_face;
}

} // namespace wmtk::autogen::utils
