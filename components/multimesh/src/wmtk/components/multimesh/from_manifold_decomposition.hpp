#pragma once

#include <array>
#include <map>
#include <memory>
#include <string_view>
#include <vector>
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk {
class Mesh;
namespace utils::internal {
template <size_t Dim>
struct ManifoldDecomposition;
}
namespace attribute {
class MeshAttributeHandle;

}
} // namespace wmtk
namespace wmtk::components::multimesh {
struct NonManifoldCascade
{
    // NonManifoldCascade(NonManifoldCascade& parent);
    NonManifoldCascade(Eigen::Ref<const MatrixXl> parent_S);
    template <size_t D>
    NonManifoldCascade(const wmtk::utils::internal::ManifoldDecomposition<D>& parent_md);
    // template <size_t D>
    // NonManifoldCascade(const wmtk::utils::internal::ManifoldDecomposition<D>& md);
    template <int D>
    void init(Eigen::Ref<const RowVectors<int64_t, D>> parent_S);
    template <size_t D>
    void init(const wmtk::utils::internal::ManifoldDecomposition<D>& md);
    template <size_t D>
    void try_make_child(const wmtk::utils::internal::ManifoldDecomposition<D>& md);

    // simplices with respect to the parent mesh's vertex indices
    MatrixXl S;

    std::vector<std::array<Tuple, 2>> parent_map;

    bool empty() const { return S.rows() == 0; }

    NonManifoldCascade* m_parent = nullptr;
    std::unique_ptr<NonManifoldCascade> m_child = nullptr;
};

// returns the new root mesh (the input mesh becomes a child mesh)
// NonManifoldCascade cascade_from_manifold_decomposition(wmtk::Mesh& m);
//
// std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(
//    wmtk::Mesh& m,
//    bool flat_structure = false);


NonManifoldCascade cascade_from_manifold_decomposition(Eigen::Ref<const MatrixXl> parent_S);

std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(
    Eigen::Ref<const MatrixXl> S,
    bool flat_structure);
} // namespace wmtk::components::multimesh
