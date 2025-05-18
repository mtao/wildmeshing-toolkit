#pragma once

#include <wmtk/PrimitiveType.hpp>
#include <wmtk/Tuple.hpp>
namespace wmtk::utils::internal {

Tuple identity_tuple(PrimitiveType pt, int64_t gid = -1);
}
