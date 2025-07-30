#include "MultiMeshLinkConditionInvariant.hpp"
#include <wmtk/utils/Logger.hpp>

#include <stdexcept>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/multimesh/MultiMeshSimplexVisitor.hpp>
#include <wmtk/simplex/Simplex.hpp>
#include <wmtk/simplex/link_condition.hpp>

namespace wmtk::invariants {
namespace {

struct MultiMeshLinkConditionFunctor
{
    template <typename T>
    bool operator()(const T& m, const simplex::Simplex& s, int64_t)
    {
        return this->operator()(m, s);
    }
    template <typename MeshType>
    bool operator()(const MeshType& m, const simplex::Simplex& s) const
    {
        return m.is_free() || simplex::link_condition(m, s.tuple());
    }
};
} // namespace

MultiMeshLinkConditionInvariant::MultiMeshLinkConditionInvariant(const Mesh& m, bool smm)
    : Invariant(m, true, false, false)
    , m_single_mesh_mode(smm)
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
bool MultiMeshLinkConditionInvariant::before(const simplex::Simplex& t) const
{
    assert(t.primitive_type() == PrimitiveType::Edge);
    if (m_single_mesh_mode) {
        return MultiMeshLinkConditionFunctor{}(mesh(), t);

    } else {
        multimesh::MultiMeshSimplexVisitor visitor(
            std::integral_constant<int64_t, 1>{}, // specify that this runs on edges
            MultiMeshLinkConditionFunctor{});
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

std::string MultiMeshLinkConditionInvariant::name() const
{
    return "MultiMeshLinkConditionInvariant";
}
} // namespace wmtk::invariants
