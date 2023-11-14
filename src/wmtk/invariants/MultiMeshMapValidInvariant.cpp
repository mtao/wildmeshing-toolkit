#include "MultiMeshMapValidInvariant.hpp"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <wmtk/EdgeMesh.hpp>
#include <wmtk/Mesh.hpp>
#include <wmtk/PointMesh.hpp>
#include <wmtk/SimplicialComplex.hpp>
#include <wmtk/TetMesh.hpp>
#include <wmtk/TriMesh.hpp>
#include <wmtk/multimesh/MultiMeshVisitor.hpp>
#include <wmtk/simplex/top_dimension_cofaces.hpp>

namespace wmtk {
namespace {

bool are_all_ears_in_child(const)

    struct MultiMeshMapValidFunctor
{
    bool operator()(const Mesh& m, const simplex::Simplex& s) const { return true; }
    bool operator()(const PointMesh& m, const simplex::Simplex& s) const { return true; }

    bool operator()(const EdgeMesh& m, const simplex::Simplex& s) const { return true; }
    bool operator()(const TriMesh& m, const simplex::Simplex& s) const
    {
        const Tuple t = s.tuple();
        const std::vector<Tuple> equivalent_tuples = simplex::top_dimension_cofaces_tuples(m, t);

        return SimplicialComplex::link_cond_bd_2d(m, s.tuple());
    }
    bool operator()(const TetMesh& m, const simplex::Simplex& s) const
    {
        throw std::runtime_error("implement link condition in multimesh trimesh");
        return false;
    }
};
} // namespace

MultiMeshMapValidInvariant::MultiMeshMapValidInvariant(const Mesh& m)
    : MeshInvariant(m)
{}
bool MultiMeshMapValidInvariant::before(const Tuple& t) const
{
    multimesh::MultiMeshVisitor visitor(
        std::integral_constant<long, 1>{}, // specify that this runs on edges
        MultiMeshMapValidFunctor{});
    // TODO: fix visitor to work for const data
    auto data =
        visitor.execute_from_root(const_cast<Mesh&>(mesh()), Simplex(PrimitiveType::Edge, t));

    for (const auto& [key, value_var] : data) {
        const bool valid = std::get<bool>(value_var);
        if (!valid) {
            return false;
        }
    }
    return true;
}
} // namespace wmtk
