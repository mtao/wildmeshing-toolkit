#include "from_manifold_decomposition.hpp"
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/from_tag.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>
namespace wmtk::components::multimesh {
namespace {

template <size_t Dim>
auto tag_and_emplace_mesh(Mesh& m, const wmtk::utils::internal::ManifoldDecomposition<Dim + 1>& mfd)
{
    static_assert(Dim > 0);
    PrimitiveType pt = get_primitive_type_from_id(Dim - 1);

    auto tups = m.get_all(pt);
    const std::map<std::array<int64_t, Dim>, Tuple>& face_map = mfd.face_map;
    auto attr = m.template register_attribute<char>("temp", pt, 1);
    auto acc = attr.create_accessor<char, 1>();

    if constexpr (Dim == 1) {
    } else if constexpr (Dim == 2) {
    } else if constexpr (Dim == 3) {
        assert(false); // not implemented yet
    }


    for (const auto& [k, v] : mfd.face_map) {
        acc.scalar_attribute(v) = 1;
    }

    auto r = wmtk::components::multimesh::from_tag(attr, 1, {});
    m.delete_attribute(attr);
    return r;
}


template <size_t Dim>
std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(
    wmtk::Mesh& m,
    Eigen::Ref<const RowVectors<int64_t, Dim>> S)
{
    if constexpr (Dim > 1) {
        auto MFD = wmtk::utils::internal::boundary_manifold_decomposition<Dim>(S);

        if (MFD.face_map.size() > 0) {
            auto B = MFD.face_matrix();
            auto c = tag_and_emplace_mesh<Dim - 1>(m, MFD);
            auto F = from_manifold_decomposition<Dim - 1>(*c, MFD.face_matrix());
            F.emplace_back(c);

            return F;
        } else {
            return std::vector<std::shared_ptr<Mesh>>({m.shared_from_this()});
        }
    }

    return {};
}
} // namespace

// returns the new root mesh (the input mesh becomes a child mesh)
std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(wmtk::Mesh& m)
{
    wmtk::utils::EigenMatrixWriter writer;
    m.serialize(writer);
    MatrixXl S = writer.get_simplex_vertex_matrix();

    static_assert(MatrixXl::ColsAtCompileTime == Eigen::Dynamic);
    switch (S.cols()) {
    case 4: return from_manifold_decomposition<4>(m, S);
    case 3: return from_manifold_decomposition<3>(m, S);
    case 2: return from_manifold_decomposition<2>(m, S);
    case 1: return from_manifold_decomposition<1>(m, S);
    default: break;
    }

    return {};
}
} // namespace wmtk::components::multimesh
