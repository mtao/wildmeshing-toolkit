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
auto tag_and_emplace_mesh(Mesh& m, Eigen::Ref<const RowVectors<int64_t, Dim>> S)
{
    static_assert(Dim > 0);
    PrimitiveType pt = get_primitive_type_from_id(Dim - 1);

    auto tups = m.get_all(pt);
    std::map<std::array<int64_t, Dim>, Tuple> face_map;
    auto attr = m.template register_attribute<char>("temp", pt, 1);
    auto acc = attr.create_accessor<char, 1>();

    if constexpr (Dim == 1) {
        for (size_t j = 0; j < tups.size(); ++j) {
            face_map.emplace(j, tups[j]);
        }

    } else if constexpr (Dim == 2) {
        for (size_t j = 0; j < tups.size(); ++j) {
            auto t = tups[j];
            int64_t a = m.id(t, wmtk::PrimitiveType::Vertex);
            wmtk::Tuple t2 = m.switch_tuple(t, wmtk::PrimitiveType::Vertex);
            int64_t b = m.id(t2, wmtk::PrimitiveType::Vertex);
            face_map.emplace(j, tups[j]);

            {
                std::array<int64_t, 2> x{{a, b}};
                face_map.emplace(x, t);
            }
            {
                std::array<int64_t, 2> x{{b, a}};
                face_map.emplace(x, t2);
            }
        }
    } else if constexpr (Dim == 3) {
        assert(false); // not implemented yet
    }


    for (int j = 0; j < S.rows(); ++j) {
        auto s = S.row(j);
        std::array<int64_t, Dim> key;
        std::copy(s.begin(), s.end(), key.begin());
        acc.scalar_attribute(face_map.at(key)) = 1;
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
        auto [S2, B] = wmtk::utils::internal::boundary_manifold_decomposition<Dim>(S);

        if (B.size() > 0) {
            auto c = tag_and_emplace_mesh<Dim - 1>(m, B);
            auto F = from_manifold_decomposition<Dim - 1>(*c, B);
            F.emplace_back(m.shared_from_this());

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
