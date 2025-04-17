#include "concatenate.hpp"
namespace wmtk::components::multimesh::utils {
MatrixXl concatenate(const std::vector<MatrixXl>& container)
{
    Eigen::Index rows = 0;
    Eigen::Index cols = -1;
    for (const auto& m : container) {
        rows += m.rows();
        if (cols == -1) {
            cols = m.cols();
        } else {
            assert(cols == m.cols());
        }
    }
    MatrixXl R(rows, cols);
    R.setZero();
    Eigen::Index offset = 0;
    Eigen::Index voffset = 0;
    for (const auto& m : container) {
        auto b = R.block(offset, 0, m.rows(), m.cols());
        b = m.array() + voffset;
        voffset = b.maxCoeff() + 1;
        offset += m.rows();
    }
    return R;
}
} // namespace wmtk::components::multimesh::utils
