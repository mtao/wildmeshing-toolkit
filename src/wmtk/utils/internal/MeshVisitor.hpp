#pragma once
#include "active_simplex_indices.hpp"

namespace wmtk {
class Mesh;
}
namespace wmtk::utils::internal {

/// Proxy class to reduce the friend classing required for multimesh. This should just be invoked by each class
class MeshVisitor
{
public:
    friend std::vector<int64_t> active_simplex_indices(const Mesh& m, PrimitiveType pt);

private:
    static std::vector<int64_t> active_simplex_indices(const Mesh& m);
};
} // namespace wmtk::utils::internal
