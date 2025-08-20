#include "OperationFactory.hpp"
#include <nlohmann/json.hpp>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>
#include "../Configurator.hpp"
#include "OperationOptions.hpp"


namespace wmtk::components::configurator::operations {

namespace {
template <typename T, typename S, typename MeshType = wmtk::Mesh>
std::shared_ptr<wmtk::operations::Operation> default_add_operation(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<S>();
    auto& m = c.get_mesh<MeshType>(opts.mesh_path);
    auto r = std::make_shared<T>(m);

    if (opts.priority) {
        opts.priority.assign_to(c.meshes(), *r);
    }
    for (const auto& [name,inv] : opts.invariants) {
        r->add_invariant(c.create_invariant(name,inv));
    }
    return r;
}

} // namespace

OperationFactory::OperationFactory()
{
    m_op_functors["split"] = &default_add_operation<wmtk::operations::EdgeSplit, EdgeSplitOptions>;
    m_op_functors["collapse"] =
        &default_add_operation<wmtk::operations::EdgeCollapse, EdgeCollapseOptions>;
    m_op_functors["swap2"] = &default_add_operation<
        wmtk::operations::composite::TriEdgeSwap,
        EdgeSwapOptions,
        wmtk::TriMesh>;
}
void OperationFactory::add(const std::string& s, const OpCreatorFunc& f)
{
    m_op_functors[s] = f;
}
std::shared_ptr<wmtk::operations::Operation> OperationFactory::create(
    Configurator& config,
    const nlohmann::json& js)

{
    return create(config, js["name"].get<std::string>(), js);
}
std::shared_ptr<wmtk::operations::Operation>
OperationFactory::create(Configurator& config, std::string_view name, const nlohmann::json& js)
{
    auto r = m_op_functors.at(js["type"])(config, js);
    m_ops[std::string(name)] = r;
    return r;
}
std::shared_ptr<wmtk::operations::Operation> OperationFactory::get(std::string_view name)
{
    return m_ops.at(std::string(name));
}

void OperationFactory::from_json(Configurator& c, const nlohmann::json& js)
{
    if (js.is_array()) {
        for (const auto& j : js) {
            create(c, j);
        }
    } else {
        assert(js.is_object());
        for (const auto& [n, j] : js.items()) {
            create(c, n, j);
        }
    }
}
std::string_view OperationFactory::get_name(const wmtk::operations::Operation& op) const
{
    for (const auto& [name, my_op] : m_ops) {
        if (&op == my_op.get()) {
            return name;
        }
    }
    constexpr static std::string unknown = "unknown";
    return unknown;
}

} // namespace wmtk::components::configurator::operations
