#include "isotropic_remeshing.hpp"

#include <wmtk/components/multimesh/MeshCollection.hpp>
#include "IsotropicRemeshing.hpp"

namespace wmtk::components::isotropic_remeshing {


void isotropic_remeshing(Mesh& m, const IsotropicRemeshingOptions& options)
{
    multimesh::MeshCollection mc;
    mc.emplace_mesh(m);

    isotropic_remeshing::isotropic_remeshing(mc, options);
}
void isotropic_remeshing(multimesh::MeshCollection& mc, const IsotropicRemeshingOptions& options)
{
    IsotropicRemeshing app(mc, options);
    app.run();
}
} // namespace wmtk::components::isotropic_remeshing
