#include "PrimitiveType.hpp"
#include <string>
#include <array>


namespace wmtk {
namespace {
// NOTE: The order of these entries must be aligned with the order of enum values in PrimitiveType.
// Invalid must be the last string here for primitive_type_name to work
const static std::array<std::string,5> names = {{
    "Vertex",
    "Edge",
    "Face",
    "Tetrahedron"
    "Invalid"}};
} // namespace

int8_t get_max_primitive_type_id(const std::vector<PrimitiveType>& primitive_types)
{
    int8_t max_id = -1;
    for (const auto& t : primitive_types) {
        max_id = std::max(max_id, get_primitive_type_id(t));
    }

    return max_id;
}

// PrimitiveType get_primitive_type_from_id(int8_t id)
//{
//     switch (id) {
//     case 0: return PrimitiveType::Vertex;
//     case 1: return PrimitiveType::Edge;
//     case 2: return PrimitiveType::Face;
//     case 3: return PrimitiveType::Tetrahedron;
//     case 4: return PrimitiveType::HalfEdge;
//    default: break; // just return at the end because compilers can be finicky
//    }
//
//    return PrimitiveType::Vertex;
//}

std::string_view primitive_type_name(PrimitiveType t)
{
    int64_t id = get_primitive_type_id(t);
    if(id >= names.size()) {
        return names.back();
    } else {
        return names[id];
    }
}

} // namespace wmtk
