#pragma once
#include <wmtk/Types.hpp>

namespace wmtk {
class Mesh;
}

namespace wmtk::utils::internal {

bool is_manifold(const Mesh& m);
}

