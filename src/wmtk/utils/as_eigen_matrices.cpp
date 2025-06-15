#include "as_eigen_matrices.hpp"
#include <wmtk/Mesh.hpp>
#include "EigenMatrixWriter.hpp"

namespace wmtk::utils {

// returns a wmtk matrix in the form of an igl style position, vertex ids pair
std::pair<Eigen::MatrixXd, MatrixXl> as_eigen_matrices_with_position(
    const Mesh& m,
    std::string_view position)
{
    EigenMatrixWriter emw;
    emw.set_position_attribute_name(position);
    m.serialize(emw);
    Eigen::MatrixXd P;
    emw.get_position_matrix(P);
    return std::make_pair(P, emw.get_simplex_vertex_matrix());
}

// returns a wmtk matrix in the form of an igl style position, vertex ids pair
MatrixXl as_eigen_matrices(const Mesh& m)
{
    EigenMatrixWriter emw;
    m.serialize(emw);
    return emw.get_simplex_vertex_matrix();
}

MatrixXl as_mapped_eigen_matrices(const Mesh& m, const Mesh& child)
{
    auto child_vertices = child.get_all(wmtk::PrimitiveType::Vertex);
    std::vector<int64_t> vertex_map(child_vertices.size(), -1);
    for (const auto& cv : child_vertices) {
        auto mapped = child.map(m, wmtk::simplex::Simplex::vertex(cv));
        switch (mapped.size()) {
        case 0: continue; // keep the -1
        default:
            throw std::invalid_argument(
                "Was unable to map to other mesh because one vertx became many");
            continue;
        case 1: {
            vertex_map[child.id(cv, PrimitiveType::Vertex)] = m.id(mapped[0]);
        }
        }
    }
    auto S = as_eigen_matrices(child);
    S.noalias() = S.unaryExpr([&](int64_t value) -> int64_t { return vertex_map[value]; });
    return S;
}
} // namespace wmtk::utils
