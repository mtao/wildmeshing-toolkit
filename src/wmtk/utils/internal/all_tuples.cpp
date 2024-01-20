#include <wmtk/Mesh.hpp>
namespace wmtk::utils::internal {
std::vector<Tuple> all_tuples(int64_t size);
std::vector<Tuple> all_tuples(const std::vector<int64_t>& indices);
std::vector<Tuple> all_tuples(const Mesh& m)
{
    auto simplices = m.get_all(PrimitiveType::m.top_simplex_type());
}
} // namespace wmtk::utils::internal
