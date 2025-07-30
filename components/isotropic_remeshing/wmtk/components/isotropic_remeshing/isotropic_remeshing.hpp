#pragma once

namespace wmtk {
class Mesh;
}
namespace wmtk::components::multimesh {
class MeshCollection;
}
namespace wmtk::components::isotropic_remeshing {
struct IsotropicRemeshingOptions;

void isotropic_remeshing(Mesh&, const IsotropicRemeshingOptions& options);
void isotropic_remeshing(multimesh::MeshCollection&, const IsotropicRemeshingOptions& options);

} // namespace wmtk::components::isotropic_remeshing
