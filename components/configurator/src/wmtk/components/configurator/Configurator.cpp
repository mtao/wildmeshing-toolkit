
#include "Configurator.hpp"
#include <wmtk/operations/Operation.hpp>
#include "Configuration.hpp"

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

Configurator::Configurator()
    : m_meshes(std::make_shared<wmtk::components::multimesh::MeshCollection>())
{}
Configurator::Configurator(const nlohmann::json& js)
    : Configurator()
{
    from_json(js);
}

// call default constructor
Configurator::Configurator(const PassConfiguration& config)
    : Configurator()
{
    load(config);
}
Configurator::Configurator(const Configuration& config)
{
    load(config);
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
    m_meshes->emplace_mesh(wmtk::components::input::input(config.input));
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
    return *m_meshes;
}
const wmtk::components::multimesh::MeshCollection& Configurator::meshes() const
{
    return *m_meshes;
}
auto Configurator::get_attribute(
    const wmtk::components::multimesh::utils::AttributeDescription& attr) const
    -> wmtk::attribute::MeshAttributeHandle
{
    return wmtk::components::multimesh::utils::get_attribute(meshes(), attr);
}
} // namespace wmtk::components::configurator
