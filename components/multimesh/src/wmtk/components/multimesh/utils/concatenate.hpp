
#pragma once

#include <vector>
#include <wmtk/Types.hpp>

namespace wmtk::components::multimesh::utils {
MatrixXl concatenate(const std::vector<MatrixXl>& container);
}
