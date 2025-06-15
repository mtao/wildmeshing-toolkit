#pragma once

#include <Eigen/Core>
#include <string_view>
#include <wmtk/Types.hpp>

namespace wmtk {
class Mesh;
}
namespace wmtk::utils {

// returns a wmtk matrix in the form of an igl style position, vertex ids pair
std::pair<Eigen::MatrixXd, MatrixXl> as_eigen_matrices_with_position(
    const Mesh& m,
    std::string_view position = "vertices");
MatrixXl as_eigen_matrices(const Mesh& m);

// only works for maps where this is unique, throws otherwise
MatrixXl as_mapped_eigen_matrices(const Mesh& m, const Mesh& other);
} // namespace wmtk::utils
