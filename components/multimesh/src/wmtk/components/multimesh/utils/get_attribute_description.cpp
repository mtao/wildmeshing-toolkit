

#include "get_attribute_description.hpp"
#include <wmtk/Mesh.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include "AttributeDescription.hpp"
namespace wmtk::components::multimesh {
namespace utils {
AttributeDescription get_attribute_description(
    const MeshCollection& collection,
    const wmtk::attribute::MeshAttributeHandle& handle)
{
    auto name = collection.get_name(handle.mesh());

    const auto& nmm = collection.get_named_multimesh(name);
    return get_attribute_description(nmm, handle);
}
AttributeDescription get_attribute_description(
    const NamedMultiMesh& nmm,
    const wmtk::attribute::MeshAttributeHandle& handle)
{
    return AttributeDescription(nmm, handle);
}
} // namespace utils
} // namespace wmtk::components::multimesh
