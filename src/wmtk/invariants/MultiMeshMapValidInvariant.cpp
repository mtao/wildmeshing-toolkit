#include "MultiMeshMapValidInvariant.hpp"
#include <wmtk/utils/Logger.hpp>
#include "wmtk/dart/SimplexDart.hpp"
#include "wmtk/dart/utils/get_canonical_faces.hpp"

#include <stdexcept>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/multimesh/MultiMeshSimplexVisitor.hpp>
#include <wmtk/operations/internal/CollapseAlternateFacetOptionData.hpp>
#include <wmtk/simplex/cofaces_single_dimension_iterable.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>

#include "wmtk/simplex/cofaces_single_dimension.hpp"

namespace wmtk {
namespace {


bool both_map_to_child(
    const Mesh& parent,
    const Mesh& child,
    const dart::Dart& left,
    const dart::Dart& right)
{
    const PrimitiveType child_type = child.top_simplex_type();
    int64_t left_id = parent.id(left, child_type);
    int64_t right_id = parent.id(right, child_type);
    return left_id != right_id && (parent.can_map_child(child, left, child_type) &&
                                   parent.can_map_child(child, right, child_type));
}

// computes teh two ears in a K+1 simplex over the input edge to see if their facets will be mapped
// into one another
bool both_map_to_child(const Mesh& parent, const Mesh& child, const Tuple& input, int8_t p)
{
    const PrimitiveType child_type = child.top_simplex_type();
    const PrimitiveType parent_type = parent.top_simplex_type();
    assert(parent_type > child_type);
    const PrimitiveType collapsed_simplex_type = std::min(child_type + 1, parent_type);
    Tuple left_t =
        operations::internal::CollapseAlternateFacetOptionData::get_left_face(parent, input);
    Tuple right_t =
        operations::internal::CollapseAlternateFacetOptionData::get_right_face(parent, input);
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(parent_type);

    dart::Dart left = sd.dart_from_tuple(left_t);
    dart::Dart right = sd.dart_from_tuple(right_t);

    return both_map_to_child(parent, child, left, right);
}


// two child  K-facets will merge into one another if they are the ears of a K+1 simplex whose
// "input edge" is the input edge. This function iterates through those K+1 simplices and lets
// both_map_to_child check for if both ears are mapped
bool any_pairs_both_map_to_child(const Mesh& parent, const Mesh& child, const Tuple& edge)
{
    const PrimitiveType parent_type = parent.top_simplex_type();
    const PrimitiveType parent_face_type = parent_type - 1;
    const PrimitiveType child_type = child.top_simplex_type();
    const auto& sd = wmtk::dart::SimplexDart::get_singleton(parent_type);
    const auto& parent_face_sd = wmtk::dart::SimplexDart::get_singleton(parent_face_type);
    const auto& child_sd = wmtk::dart::SimplexDart::get_singleton(child_type);
    assert(parent_type > child_type);
    if (parent_type == child_type) {
        // if the meshes are the same dimension then there isn't a pair, so this function returns
        // false
        return false;
    } else if (parent_type == child_type + 1) {
        return both_map_to_child(parent, child, edge, sd.identity());
    } else {
        for (const int8_t f : dart::utils::get_canonical_faces(parent_face_sd, child_type)) {
            int8_t p = parent_face_sd.convert(f, sd);
            if (both_map_to_child(parent, child, edge, p)) {
                return true;
            }
        }
    }
    return false;
}


struct MultiMeshMapValidFunctor
{
    template <typename T>
    bool operator()(const T& m, const simplex::Simplex& s, int64_t)
    {
        return this->operator()(m, s);
    }
    bool operator()(const Mesh& m, const simplex::Simplex& s) const
    {
        auto cofaces = wmtk::simplex::top_dimension_cofaces_tuples(m, s);
        for (auto child_ptr : m.get_child_meshes()) {
            for (const auto& t : cofaces) {
                if (any_pairs_both_map_to_child(m, *child_ptr, t)) {
                    return false;
                }
            }
        }
        return true;
    }
};
} // namespace

MultiMeshMapValidInvariant::MultiMeshMapValidInvariant(const Mesh& m, bool single_mesh_mode)
    : Invariant(m, true, false, false)
    , m_single_mesh_mode((single_mesh_mode))
{
    if (m.is_multi_mesh_root() && !m.has_child_mesh()) {
        if (!m_single_mesh_mode) {
            wmtk::logger().debug(
                "Link condition is automatically being set to single mesh mode beacuse mesh does "
                "not have a multimesh");
        }
        m_single_mesh_mode = true;
    }
}
bool MultiMeshMapValidInvariant::before(const simplex::Simplex& t) const
{
    assert(t.primitive_type() == PrimitiveType::Edge);
    if (m_single_mesh_mode) {
        return MultiMeshMapValidFunctor{}(mesh(), t);
    } else {
        multimesh::MultiMeshSimplexVisitor visitor(
            std::integral_constant<int64_t, 1>{}, // specify that this runs on edges
            MultiMeshMapValidFunctor{});
        // TODO: fix visitor to work for const data
        visitor.execute_from_root(
            const_cast<Mesh&>(mesh()),
            simplex::NavigatableSimplex(mesh(), t));
        const auto& data = visitor.cache();

        for (const auto& [key, value_var] : data) {
            const bool valid = std::get<bool>(value_var);
            if (!valid) {
                return false;
            }
        }
        return true;
    }
}
std::string MultiMeshMapValidInvariant::name() const
{
    return "MultiMeshMapValidInvariant";
}
} // namespace wmtk
