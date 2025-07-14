#include "Envelope.hpp"
namespace wmtk::components::envelope {

Envelope::~Envelope() = default;

bool Envelope::is_inside(const Mesh& m, const std::vector<Tuple>& simplices) const
{
    return is_inside(m, simplices, m.top_simplex_type());
}
bool Envelope::is_inside(const Mesh& m, const std::vector<Tuple>& simplices, PrimitiveType pt) const
{
    for (const auto& s : simplices) {
        if (!is_inside(m, s, pt)) {
            return false;
            ;
        }
    }
    return true;
}

} // namespace wmtk::components::envelope
