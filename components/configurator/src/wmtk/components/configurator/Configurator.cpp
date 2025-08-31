
#include "Configurator.hpp"
#include <wmtk/components/multimesh/utils/get_attribute_description.hpp>
#include <wmtk/operations/Operation.hpp>
#include "Configuration.hpp"
#include "invariants/InvariantOptions.hpp"

#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/input/input.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/components/multimesh/utils/get_attribute.hpp>

namespace wmtk::components::configurator {

auto Configurator::get_operation_internal(std::string_view op_name)
    -> std::shared_ptr<wmtk::operations::Operation>
{
    return m_operations.get(op_name);
}

Configurator::Configurator(multimesh::MeshCollection& mc, const nlohmann::json& js)
    : Configurator(mc)
{
    from_json(js);
}

Configurator::Configurator(multimesh::MeshCollection& mc)
    : m_meshes(mc)
{}
Configurator::Configurator(multimesh::MeshCollection& mc, const PassConfiguration& config)
    : Configurator(mc)
{
    load(config);
}
Configurator::Configurator(multimesh::MeshCollection& mc, const Configuration& config)
    : Configurator(mc)
{
    load(config);
}

std::string_view Configurator::get_invariant_name(const wmtk::invariants::Invariant& op) const
{
    return m_invariants.get_name(op);
}
std::string_view Configurator::get_operation_name(const wmtk::operations::Operation& op) const
{
    return m_operations.get_name(op);
}
std::string Configurator::get_mesh_name(const Mesh& m) const
{
    return meshes().get_mesh_path(m);
}


WMTK_NLOHMANN_JSON_FRIEND_TO_JSON_PROTOTYPE(Configurator) {}
WMTK_NLOHMANN_JSON_FRIEND_FROM_JSON_PROTOTYPE(Configurator)
{
    nlohmann_json_t.from_json(nlohmann_json_j);
}

void Configurator::load(const Configuration& config)
{
    m_meshes.emplace_mesh(wmtk::components::input::input(config.input));
    load(static_cast<const PassConfiguration&>(config));
}
void Configurator::load(const PassConfiguration& config)
{
    for (const auto& [n, i] : config.invariants) {
        m_invariants.create(*this, n, i);
    }

    for (const auto& [n, o] : config.operations) {
        m_operations.create(*this, n, o);
    }


    for (const auto& p : config.passes) {
        m_passes.emplace_back(*this, p);
    }
}
void Configurator::from_json(const nlohmann::json& js)
{
    load(js.get<Configuration>());
}

std::shared_ptr<wmtk::invariants::Invariant> Configurator::create_invariant(
    std::string_view name,
    const nlohmann::json& js)
{
    return m_invariants.create(*this, name, js);
}

std::shared_ptr<wmtk::invariants::Invariant> Configurator::create_invariant(
    std::string_view name,
    const invariants::InvariantOptions& opts)
{
    return m_invariants.create(*this, name, opts);
}

std::shared_ptr<wmtk::operations::Operation> Configurator::create_operation(
    std::string_view name,
    const nlohmann::json& js)
{
    return m_operations.create(*this, name, js);
}

std::shared_ptr<wmtk::operations::Operation> Configurator::create_operation(
    const nlohmann::json& js)
{
    return m_operations.create(*this, js);
}

auto Configurator::get_mesh_internal(std::string_view name) -> wmtk::Mesh&
{
    return meshes().get_mesh(name);
}
auto Configurator::get_mesh_internal(std::string_view name) const -> const wmtk::Mesh&
{
    return meshes().get_mesh(name);
}

wmtk::components::multimesh::MeshCollection& Configurator::meshes()
{
    return m_meshes;
}
const wmtk::components::multimesh::MeshCollection& Configurator::meshes() const
{
    return m_meshes;
}
auto Configurator::get_attribute(
    const wmtk::components::multimesh::utils::AttributeDescription& attr) const
    -> wmtk::attribute::MeshAttributeHandle
{
    return wmtk::components::multimesh::utils::get_attribute(meshes(), attr);
}
auto Configurator::create_mesh_invariant(
        std::string_view name,
        std::string_view type,
         const Mesh& m)
    -> std::shared_ptr<wmtk::invariants::Invariant>
{
    invariants::MeshInvariantParameters p;
    p.mesh_path = get_mesh_name(m);
    invariants::InvariantOptions opts;
    opts.type = type;
    opts.parameters = p;
    return m_invariants.create(*this, name, opts);
}
auto Configurator::create_attribute_invariant(
        std::string_view name,
        std::string_view type,
    const attribute::MeshAttributeHandle& mah) -> std::shared_ptr<wmtk::invariants::Invariant>
{
    invariants::InvariantOptions opts;
    opts.type = type;
    invariants::AttributeInvariantParameters p;
    opts.type = "attribute";
    p.attribute = wmtk::components::multimesh::utils::get_attribute_handle(meshes(), mah);
    opts.parameters = p;
    return m_invariants.create(*this, name, opts);
}

} // namespace wmtk::components::configurator
