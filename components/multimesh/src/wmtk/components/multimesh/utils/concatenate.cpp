#include "concatenate.hpp"
namespace wmtk::components::multimesh::utils {
// concatenates a dart orientation map
std::vector<std::array<Tuple, 2>> concatenate(
    std::span<const std::vector<std::array<Tuple, 2>>> container)
{
    size_t total_size = 0;
    for (const auto& c : container) {
        total_size += c.size();
    }

    std::vector<std::array<Tuple, 2>> total;
    total.reserve(total_size);
    int64_t offset = 0;
    for (const auto& c : container) {
        for (const auto& [a, b] : c) {
            assert(a.global_cid() < c.size()); // The left values of the tuple should map from every
                                               // simplex in the child mesh to every simplex. We aer
                                               // assuming everything is compressed here
            int8_t lvid = a.local_vid();
            int8_t leid = a.local_eid();
            int8_t lfid = a.local_fid();
            int64_t gid = offset + a.global_cid();
            Tuple ap(lvid, leid, lfid, gid);
            total.emplace_back(std::array<Tuple, 2>{{ap, b}});
        }
        offset += c.size();
    }
    return total;
}

MatrixXl concatenate(std::span<const MatrixXl> container)
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
