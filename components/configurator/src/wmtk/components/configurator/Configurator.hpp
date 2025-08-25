#pragma once
#include <memory>
#include <nlohmann/json.hpp>
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
    Configurator(multimesh::MeshCollection& mc);
    Configurator(multimesh::MeshCollection& mc, const nlohmann::json& js);
    Configurator(multimesh::MeshCollection& mc, const PassConfiguration& config);
    Configurator(multimesh::MeshCollection& mc, const Configuration& config);

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

    std::string_view get_invariant_name(const wmtk::invariants::Invariant& op) const;

    const std::vector<Pass>& get_passes() const { return m_passes; }
    std::vector<Pass>& get_passes() { return m_passes; }


    template <typename T, typename MeshType = wmtk::Mesh>
    void add_mesh_invariant(const std::string& s);
    template <typename T, typename MeshType = wmtk::Mesh>
    void add_attribute_invariant(const std::string& s);

    std::shared_ptr<wmtk::invariants::Invariant> create_mesh_invariant(
        std::string_view s,
        const Mesh& m);
    std::shared_ptr<wmtk::invariants::Invariant> create_attribute_invariant(
        const std::string& s,
        const attribute::MeshAttributeHandle& mah);

    template <
        typename T,
        typename S = invariants::MeshInvariantOptions,
        typename MeshType = wmtk::Mesh>
    void add_basic_operation(const std::string& s);

private:
    wmtk::Mesh& get_mesh_internal(std::string_view name);
    const wmtk::Mesh& get_mesh_internal(std::string_view name) const;

    auto get_operation_internal(std::string_view op_name)
        -> std::shared_ptr<wmtk::operations::Operation>;


    wmtk::components::multimesh::MeshCollection& m_meshes;
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

template <typename T, typename MeshType>
void Configurator::add_mesh_invariant(const std::string& s)
{
    auto func = [](Configurator& c,
                   const nlohmann::json& js) -> std::shared_ptr<wmtk::invariants::Invariant> {
        auto opts = js.template get<invariants::MeshInvariantOptions>();
        auto& m = c.template get_mesh<MeshType>(opts.mesh_path);

        auto r = std::make_shared<T>(m);
        return r;
    };
    m_invariants.add(s, func);
}
template <typename T, typename MeshType>

void Configurator::add_attribute_invariant(const std::string& s)
{
    auto func = [](Configurator& c,
                   const nlohmann::json& js) -> std::shared_ptr<wmtk::invariants::Invariant> {
        auto opts = js.template get<invariants::AttributeInvariantOptions>();
        auto& m = c.template get_mesh<MeshType>(opts.attribute);

        auto r = std::make_shared<T>(m);
        return r;
    };
    m_invariants.add(s, func);
}

template <typename T, typename S, typename MeshType>
void Configurator::add_basic_operation(const std::string& s)
{
    auto func = [](Configurator& c,
                   const nlohmann::json& js) -> std::shared_ptr<wmtk::operations::Operation> {
        auto opts = js.template get<S>();
        auto& m = c.template get_mesh<MeshType>(opts.mesh_path);

        auto r = std::make_shared<T>(m);
        return r;
    };
    m_invariants.add(s, func);
}

} // namespace wmtk::components::configurator
