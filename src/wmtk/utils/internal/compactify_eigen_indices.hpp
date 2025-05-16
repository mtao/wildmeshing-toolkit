#pragma once
#include <Eigen/Core>
#include <map>

namespace wmtk::utils::internal {

template <typename Derived>
auto compactify_eigen_indices(const Eigen::MatrixBase<Derived>& M)
{
    using Scalar = typename Derived::Scalar;
    std::vector<Scalar> unindexer;
    unindexer.reserve(M.size());
    std::map<Scalar, Scalar> indexer;
    for (Eigen::Index r = 0; r < M.rows(); ++r) {
        for (Eigen::Index c = 0; c < M.cols(); ++c) {
            indexer.try_emplace(M(r, c), indexer.size());
        }
    }
    return M.unaryExpr([&](Scalar i) -> int64_t { return indexer.at(i); }).eval();
}
} // namespace wmtk::utils::internal
