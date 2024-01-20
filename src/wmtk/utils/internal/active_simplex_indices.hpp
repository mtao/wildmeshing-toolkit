#pragma once
#include <vector>

namespace wmtk {
class Mesh;
}
namepace wmtk::utils::internal
{
    std::vector<int64_t> active_simplex_indices(const Mesh& m, PrimitiveType pt);
}
