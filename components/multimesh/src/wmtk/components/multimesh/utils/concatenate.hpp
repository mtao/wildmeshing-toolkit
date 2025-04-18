
#pragma once

#include <span>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>

namespace wmtk::components::multimesh::utils {
MatrixXl concatenate(std::span<const MatrixXl> container);
std::vector<std::array<Tuple, 2>> concatenate(
    std::span<const std::vector<std::array<Tuple, 2>>> container);
} // namespace wmtk::components::multimesh::utils
