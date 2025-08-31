#include "OperationFactory.hpp"
#include <nlohmann/json.hpp>
#include <ranges>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/operations/AttributesUpdate.hpp>
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/operations/composite/TetEdgeSwap.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>
#include <wmtk/utils/Logger.hpp>
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
    for (const auto& [name, inv] : opts.invariants) {
        r->add_invariant(c.create_invariant(name, inv));
    }
    return r;
}
std::shared_ptr<wmtk::operations::Operation> add_swap_operation(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<EdgeSwapOptions>();
    auto& m_ = c.get_mesh<wmtk::Mesh>(opts.mesh_path);

    int8_t dim = m_.top_cell_dimension();
    std::shared_ptr<wmtk::operations::composite::EdgeSwap> r;
    assert(dim == 2 || dim == 3);
    if (dim == 2) {
        auto& m = c.get_mesh<wmtk::TriMesh>(opts.mesh_path);
        r = std::make_shared<wmtk::operations::composite::TriEdgeSwap>(m);
    } else {
        auto& m = c.get_mesh<wmtk::TetMesh>(opts.mesh_path);
        r = std::make_shared<wmtk::operations::composite::TetEdgeSwap>(m);
    }


    if (opts.priority) {
        opts.priority.assign_to(c.meshes(), *r);
    }
    for (const auto& [name, inv] : opts.invariants) {
        r->add_invariant(c.create_invariant(name, inv));
    }
    return r;
}

} // namespace

OperationFactory::OperationFactory()
{
    add("edge_split", &default_add_operation<wmtk::operations::EdgeSplit, EdgeSplitOptions>);
    add("edge_collapse",
        &default_add_operation<wmtk::operations::EdgeCollapse, EdgeCollapseOptions>);
    add("vertex_smooth",
        &default_add_operation<wmtk::operations::AttributesUpdate, VertexSmoothOptions>);
    add("edge_swap2",
        &default_add_operation<
            wmtk::operations::composite::TriEdgeSwap,
            EdgeSwapOptions,
            wmtk::TriMesh>);
    add("edge_swap3",
        &default_add_operation<
            wmtk::operations::composite::TetEdgeSwap,
            EdgeSwapOptions,
            wmtk::TetMesh>);

    add("edge_swap", &add_swap_operation);
    // m_op_functors["attr_update"] =
    //     &default_add_operation<wmtk::operations::AttributeUpdate, EdgeSwapOptions,
    //     wmtk::TetMesh>;
}

std::vector<std::string> OperationFactory::known_operation_functors() const
{
    auto tmp = std::views::transform(m_op_functors, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
}
std::vector<std::string> OperationFactory::known_operations() const
{
    auto tmp = std::views::transform(m_ops, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
}
void OperationFactory::add(const std::string& s, const OpCreatorFunc& f)
{
    m_op_functors[s] = f;
    spdlog::warn("Added op functor \"{}\" among {} available", s, known_operation_functors());
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
    // spdlog::info("{}", js.dump(2));
    wmtk::logger().debug("Creating a {} operation named {}", js["type"].get<std::string>(), name);
    std::string type = js["type"];
    try {
        auto r = m_op_functors.at(type)(config, js);
        m_ops[std::string(name)] = r;
        return r;
    } catch (const std::exception& e) {
        spdlog::warn(
            "Was unable to create op functor \"{}\" among {} available",
            type,
            known_operation_functors());
        throw e;
    }
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

std::vector<OperationOptions> OperationFactory::get_options(const Configurator& c) const
{
    std::vector<OperationOptions> opts;
    for (const auto& [opname, op] : m_ops) {
        auto& opt = opts.emplace_back();
        for (const auto& inv : op->invariants().invariants()) {
            std::string inv_name =std::string( c.get_invariant_name(*inv));
                invariants::InvariantOptions opts;
                opts.type = "alias";
                opts.paremeters = invariants::AliasInvariantParameters(inv_name);
            opt.invariants[inv_name] = opts;
        }
    }
    return opts;
}

} // namespace wmtk::components::configurator::operations
