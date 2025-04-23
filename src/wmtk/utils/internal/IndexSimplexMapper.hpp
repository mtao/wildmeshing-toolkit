#pragma once
#include <map>
#if defined(_cpp_lib_span)
#include <span>
#endif
#include <wmtk/Tuple.hpp>
#include <wmtk/Types.hpp>
#include <wmtk/dart/Dart.hpp>

namespace wmtk {
class Mesh;
class EdgeMesh;
class TriMesh;
class TetMesh;
namespace simplex {
class IdSimplex;
}
} // namespace wmtk
namespace wmtk::utils::internal {
class IndexSimplexMapper
{
public:
    IndexSimplexMapper(Eigen::Ref<const MatrixXl> S);
    IndexSimplexMapper(const Mesh& mesh);


    // TOOD: this is just an array of size nCr, but easier to just say vector for now
    template <int Dim, int ChildDim>
    std::vector<int64_t> faces(size_t index) const;

    std::vector<int64_t> faces(size_t index, int8_t dim, int8_t child_dim) const;

    static int64_t id(const simplex::IdSimplex& s);

private:
    void initialize_edge_mesh(Eigen::Ref<const RowVectors2l> S);
    void initialize_tri_mesh(Eigen::Ref<const RowVectors3l> S);
    void initialize_tet_mesh(Eigen::Ref<const RowVectors4l> S);


#if defined(_cpp_lib_span)
    template <int Dim>
    static auto get_input_dart(
        const std::span<int64_t, Dim>& s,
        const std::map<std::array<int64_t, Dim>, wmtk::dart::Dart>& mp) -> const dart::Dart&;
#endif
    template <int Dim>
    static auto get_input_dart(
        const std::array<int64_t, Dim>& s,
        const std::map<std::array<int64_t, Dim>, wmtk::dart::Dart>& mp) -> const dart::Dart&;
    template <int Dim>
    static auto get_input_dart(
        Eigen::Ref<const RowVector<int64_t, Dim>> S,
        const std::map<std::array<int64_t, Dim>, wmtk::dart::Dart>& mp) -> const dart::Dart&;


    template <int Dim, int ChildDim>
    std::map<std::array<int64_t, ChildDim>, wmtk::dart::Dart> make_child_map(
        std::vector<std::array<int64_t, Dim>> S) const;
    template <int Dim>
    static std::map<std::array<int64_t, Dim>, wmtk::dart::Dart> make_map(
        std::vector<std::array<int64_t, Dim>> S);

    int8_t m_simplex_dimension;

    std::map<std::array<int64_t, 1>, wmtk::dart::Dart> m_V_map;
    std::map<std::array<int64_t, 2>, wmtk::dart::Dart> m_E_map;
    std::map<std::array<int64_t, 3>, wmtk::dart::Dart> m_F_map;
    std::map<std::array<int64_t, 4>, wmtk::dart::Dart> m_T_map;


    void update_simplices();
    std::vector<std::array<int64_t, 1>> m_V;
    std::vector<std::array<int64_t, 2>> m_E;
    std::vector<std::array<int64_t, 3>> m_F;
    std::vector<std::array<int64_t, 4>> m_T;

    Mesh const* m_mesh = nullptr;

public:
    template <int D>
    const std::map<std::array<int64_t, D + 1>, wmtk::dart::Dart>& simplex_dart_map() const;
    template <int D>
    std::map<std::array<int64_t, D + 1>, int64_t> simplex_map() const;
    template <int D>
    const std::vector<std::array<int64_t, D + 1>>& simplices() const;

#if defined(_cpp_lib_span)
    template <size_t Dim>
    int64_t get_index(const std::span<int64_t, Dim>& s) const;

    template <size_t Dim>
    dart::Dart get_dart(const std::span<int64_t, Dim>& s) const;

    template <size_t Dim>
    wmtk::Tuple get_tuple(const std::span<int64_t, Dim>& s) const;
#endif

    template <size_t Dim>
    int64_t get_index(const std::array<int64_t, Dim>& s) const;

    template <size_t Dim>
    dart::Dart get_internal_dart(const std::array<int64_t, Dim>& s) const;

    template <size_t Dim>
    dart::Dart get_dart(const std::array<int64_t, Dim>& s) const;

    template <size_t Dim>
    wmtk::Tuple get_tuple(const std::array<int64_t, Dim>& s) const;
};

template <int D>
std::map<std::array<int64_t, D + 1>, int64_t> IndexSimplexMapper::simplex_map() const
{
    std::map<std::array<int64_t, D + 1>, int64_t> R;
    const auto& sdm = simplex_dart_map<D>();
    std::transform(sdm.begin(), sdm.end(), std::inserter(R, R.end()), [](const auto& pr) {
        return std::make_pair(pr.first, pr.second.global_id());
    });
    return R;
}

} // namespace wmtk::utils::internal
