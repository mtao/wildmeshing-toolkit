#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <wmtk/attribute/MeshAttributeHandle.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include "Pass.hpp"
#include "PassOptions.hpp"
#include "invariants/InvariantFactory.hpp"
#include "operations/OperationFactory.hpp"


namespace wmtk::components::multimesh {
namespace utils {
class AttributeDescription;
}
} // namespace wmtk::components::multimesh
namespace wmtk {
class Mesh;
namespace operations {
class Operation;
}
namespace invariants {
class Invariant;
}
} // namespace wmtk


namespace wmtk::components::configurator {

struct Configuration;
struct PassConfiguration;

class Configurator
{
public:
    Configurator();
    Configurator(const nlohmann::json& js);
    Configurator(const PassConfiguration& config);
    Configurator(const Configuration& config);

    PassConfiguration as_configuration() const;
    void from_json(const nlohmann::json& js);
    void load(const Configuration& config);
    void load(const PassConfiguration& config);
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(Configurator)
    // Configurator(wmtk::components::multimesh::MeshCollection& mc);
    std::shared_ptr<wmtk::invariants::Invariant> create_invariant(
        std::string_view name,
        const invariants::InvariantOptions& opts);
    std::shared_ptr<wmtk::invariants::Invariant> create_invariant(
        std::string_view name,
        const nlohmann::json& js);
    std::shared_ptr<wmtk::operations::Operation> create_operation(
        std::string_view name,
        const nlohmann::json& js);
    std::shared_ptr<wmtk::operations::Operation> create_operation(const nlohmann::json& js);


    wmtk::components::multimesh::MeshCollection& meshes();
    const wmtk::components::multimesh::MeshCollection& meshes() const;


    template <typename MeshType = wmtk::Mesh>
    auto get_mesh(std::string_view name) -> MeshType&;
    template <typename MeshType = wmtk::Mesh>
    auto get_mesh(std::string_view name) const -> const MeshType&;

    auto get_attribute(const wmtk::components::multimesh::utils::AttributeDescription& attr) const
        -> wmtk::attribute::MeshAttributeHandle;

    template <typename T = wmtk::operations::Operation>
    auto get_operation(std::string_view op_name) -> std::shared_ptr<T>;

    std::string_view get_operation_name(const wmtk::operations::Operation& op) const;
    std::string get_mesh_name(const Mesh& m) const;

    const std::vector<Pass>& get_passes() const { return m_passes; }
    std::vector<Pass>& get_passes() { return m_passes; }

private:
    wmtk::Mesh& get_mesh_internal(std::string_view name);
    const wmtk::Mesh& get_mesh_internal(std::string_view name) const;

    auto get_operation_internal(std::string_view op_name)
        -> std::shared_ptr<wmtk::operations::Operation>;


    std::shared_ptr<wmtk::components::multimesh::MeshCollection> m_meshes;
    operations::OperationFactory m_operations;
    invariants::InvariantFactory m_invariants;

    std::vector<Pass> m_passes;
};


template <typename MeshType>
auto Configurator::get_mesh(std::string_view name) -> MeshType&
{
    auto* m = &get_mesh_internal(name);

    MeshType* mptr;
    if constexpr (std::is_same_v<MeshType, wmtk::Mesh>) {
        mptr = m;
    } else {
        mptr = dynamic_cast<MeshType*>(m);
    }
    if (!bool(mptr)) {
        throw std::runtime_error("Could not make invariant because mesh type was wrong");
    }
    return *mptr;
}
template <typename MeshType>
auto Configurator::get_mesh(std::string_view name) const -> const MeshType&
{
    auto* m = &get_mesh_internal(name);

    MeshType const* mptr;
    if constexpr (std::is_same_v<MeshType, wmtk::Mesh>) {
        mptr = m;
    } else {
        mptr = dynamic_cast<MeshType const*>(m);
    }
    if (!bool(mptr)) {
        throw std::runtime_error("Could not fetch a mesh because mesh type was wrong");
    }
    return *mptr;
}
template <typename T>
auto Configurator::get_operation(std::string_view op_name) -> std::shared_ptr<T>
{
    auto basic_ptr = get_operation_internal(op_name);

    auto ptr = std::dynamic_pointer_cast<T>(basic_ptr);
    if (!bool(ptr)) {
        throw std::runtime_error("Could not fetch a mesh because type was wrong");
    }
    return ptr;
}

} // namespace wmtk::components::configurator
