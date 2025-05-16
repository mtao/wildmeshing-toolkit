#include "from_manifold_decomposition.hpp"
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/components/multimesh/from_tag.hpp>
#include <wmtk/utils/EigenMatrixWriter.hpp>
#include <wmtk/utils/internal/compactify_eigen_indices.hpp>
#include <wmtk/utils/internal/manifold_decomposition.hpp>
namespace wmtk::components::multimesh {
/*
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
*/

NonManifoldCascade::NonManifoldCascade(Eigen::Ref<const MatrixXl> SS)
{
    switch (SS.cols()) {
    case 4: init<4>(SS); break;
    case 3: init<3>(SS); break;
    case 2: init<2>(SS); break;
    case 1: // init<1>(SS); break;
    default: break;
    }
}
// template <size_t D>
// NonManifoldCascade(Eigen::Ref<RowVectors<int64_t, D>> parent_S)
//{}
template <size_t D>
void NonManifoldCascade::init(const wmtk::utils::internal::ManifoldDecomposition<D>& parent_md)
{
    assert(!parent_md.face_map.empty());

    parent_map = parent_md.mm_map();
    S = parent_md.face_matrix(true);


    auto md = wmtk::utils::internal::boundary_manifold_decomposition<D - 1>(S);
    try_make_child(md);
}
template <int D>
void NonManifoldCascade::init(Eigen::Ref<const RowVectors<int64_t, D>> parent_S)
{
    auto md = wmtk::utils::internal::boundary_manifold_decomposition<D>(parent_S);

    S = md.manifold_decomposition;
    try_make_child(md);
}
template <size_t D>
void NonManifoldCascade::try_make_child(const wmtk::utils::internal::ManifoldDecomposition<D>& md)
{
    if (!md.face_map.empty()) {
        m_child = std::make_unique<NonManifoldCascade>(md);
        m_child->m_parent = this;
    }
}
NonManifoldCascade cascade_from_manifold_decomposition(wmtk::Mesh& m)
{
    wmtk::utils::EigenMatrixWriter writer;
    m.serialize(writer);
    MatrixXl S = writer.get_simplex_vertex_matrix();

    return NonManifoldCascade(S);
    /*
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
*/
}
// returns the new root mesh (the input mesh becomes a child mesh)
std::vector<std::shared_ptr<Mesh>> from_manifold_decomposition(wmtk::Mesh& m, bool flat_structure)
{
    Mesh* cur_parent_mesh_ptr = &m;
    std::vector<std::shared_ptr<Mesh>> ret;
    const NonManifoldCascade nmc = cascade_from_manifold_decomposition(m);
    const auto& mesh_sd = dart::SimplexDart::get_singleton(m.top_simplex_type());


    //

    for (NonManifoldCascade const* cur_ptr = &nmc; cur_ptr != nullptr && !cur_ptr->empty();
         cur_ptr = cur_ptr->m_child.get()) {
        const NonManifoldCascade& cur = *cur_ptr;

        const PrimitiveType cur_pt = get_primitive_type_from_id(cur_ptr->S.cols() - 1);


        auto cS = wmtk::utils::internal::compactify_eigen_indices(cur.S);
        auto& mptr = ret.emplace_back(Mesh::from_vertex_indices(cS));

        // holds the current mesh being mapped to - if flat this is always &m
        Mesh& parent_mesh = *cur_parent_mesh_ptr;

        if (flat_structure) {
            std::vector<std::array<Tuple, 2>> tups = cur.parent_map;
            assert(&m == &parent_mesh.get_multi_mesh_root());
            for (auto& t : tups) {
                auto& a = t[0];
                a = parent_mesh.map_to_root(simplex::Simplex(cur_pt, a)).tuple();
            }

            m.register_child_mesh(mptr, tups);
        } else {
            parent_mesh.register_child_mesh(mptr, cur.parent_map);
        }

        // update which mesh we are looking at because we always want to link to the parent mesh
        cur_parent_mesh_ptr = mptr.get();
    }
    return ret;
}
} // namespace wmtk::components::multimesh
