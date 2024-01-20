#pragma once

#include <wmtk/Tuple.hpp>

#include <vector>

namespace wmtk {
    class Mesh;
}
namespace wmtk::utils::internal {
    std::vector<Tuple> all_tuples(int64_t size);
    std::vector<Tuple> all_tuples(const std::vector<int64_t>& indices);
    std::vector<Tuple> all_tuples(const Mesh& m);
}
