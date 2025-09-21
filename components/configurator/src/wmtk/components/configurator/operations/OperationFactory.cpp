#include "OperationFactory.hpp"
#include <nlohmann/json.hpp>
#include <ranges>
#include <wmtk/components/multimesh/MeshCollection.hpp>
#include <wmtk/operations/AttributeUpdate.hpp>
//
#include <wmtk/operations/EdgeCollapse.hpp>
#include <wmtk/operations/EdgeSplit.hpp>
#include <wmtk/operations/Operation.hpp>
#include <wmtk/operations/attribute_update/AttributeTransferStrategy.hpp>
#include <wmtk/operations/composite/ProjectOperation.hpp>
#include <wmtk/operations/composite/TetEdgeSwap.hpp>
#include <wmtk/operations/composite/TriEdgeSwap.hpp>
#include <wmtk/operations/utils/VertexLaplacianSmooth.hpp>
#include <wmtk/operations/utils/VertexTangentialLaplacianSmooth.hpp>
//
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
    auto params = opts.get_parameters();
    auto& m = c.get_mesh<MeshType>(params.mesh_path);
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
    auto params = opts.get_parameters();
    auto& m_ = c.get_mesh<wmtk::Mesh>(params.mesh_path);

    int8_t dim = m_.top_cell_dimension();
    std::shared_ptr<wmtk::operations::composite::EdgeSwap> r;
    assert(dim == 2 || dim == 3);
    if (dim == 2) {
        auto& m = c.get_mesh<wmtk::TriMesh>(params.mesh_path);
        r = std::make_shared<wmtk::operations::composite::TriEdgeSwap>(m);
    } else {
        auto& m = c.get_mesh<wmtk::TetMesh>(params.mesh_path);
        r = std::make_shared<wmtk::operations::composite::TetEdgeSwap>(m);
    }


    if (opts.priority) {
        opts.priority.assign_to(c.meshes(), *r);
    }
    for (const auto& [name, inv] : opts.invariants) {
        r->add_invariant(c.create_invariant(name, inv));
    }
    for (const auto& [name, inv] : params.split_invariants) {
        r->split().add_invariant(c.create_invariant(name, inv));
    }
    for (const auto& [name, inv] : opts.invariants) {
        r->collapse().add_invariant(c.create_invariant(name, inv));
    }
    return r;
}


std::shared_ptr<wmtk::operations::AttributeUpdate> default_add_attribute_update_function(
    Configurator& c,
    const nlohmann::json& js)
{
    auto opts = js.template get<AttributeUpdateOptions>();
    auto params = opts.get_parameters();
    auto attr = c.get_attribute(params.attribute);
    auto& mesh = attr.mesh();

    int8_t dim = mesh.top_cell_dimension();
    std::shared_ptr<wmtk::operations::AttributeUpdate> r;
    std::shared_ptr<wmtk::operations::AttributeUpdateWithFunction> op_smooth =
        std::make_shared<wmtk::operations::AttributeUpdateWithFunction>(mesh);

    op_smooth->set_function(c.create_attribute_update_function(params.function, params));


    if (!params.projection_attribute.empty()) {
        std::shared_ptr<wmtk::operations::composite::ProjectOperation> proj_op;
        proj_op = std::make_shared<wmtk::operations::composite::ProjectOperation>(op_smooth);
        auto proj_attr = c.get_attribute(params.attribute);
        proj_op->add_constraint(proj_attr, proj_attr);
        r = proj_op;
    } else {
        r = op_smooth;
    }



    if (opts.priority) {
        opts.priority.assign_to(c.meshes(), *r);
    }
    for (const auto& [name, inv] : opts.invariants) {
        r->add_invariant(c.create_invariant(name, inv));
    }
    assert(bool(r));
    return r;
}
template <typename T>
auto default_add_attribute(const Configurator& c, const nlohmann::json& js)
    -> wmtk::operations::AttributeUpdateWithFunction::UpdateFunction
{
    auto params = js.template get<AttributeUpdateOptions::Parameters>();
    auto attr = c.get_attribute(params.attribute);
    return T(attr);
}
} // namespace

OperationFactory::OperationFactory()
{
    add("edge_split", &default_add_operation<wmtk::operations::EdgeSplit, EdgeSplitOptions>);
    add("edge_collapse",
        &default_add_operation<wmtk::operations::EdgeCollapse, EdgeCollapseOptions>);
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

    add_attribute_function(
        "vertex_smooth",
        &default_add_attribute<wmtk::operations::VertexLaplacianSmooth>);

    add_attribute_function(
        "tangential_vertex_smooth",
        &default_add_attribute<wmtk::operations::VertexTangentialLaplacianSmooth>);

    // add("vertex_smooth",
    //         &add_vertex_smooth_operation);
    add("attr_update", &default_add_attribute_update_function);
}

std::vector<std::string> OperationFactory::known_operation_functors() const
{
    auto tmp = std::views::transform(m_op_functors, [](const auto& pr) { return pr.first; });
    std::vector<std::string> ret;
    std::ranges::copy(tmp, std::back_inserter(ret));
    return ret;
}
std::vector<std::string> OperationFactory::known_attribute_update_functors() const
{
    auto tmp = std::views::transform(m_attr_op_functors, [](const auto& pr) { return pr.first; });
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
    logger().debug("Added op functor \"{}\" among {} available", s, known_operation_functors());
}
void OperationFactory::add_attribute_function(
    const std::string& s,
    const AttrUpdateOpCreatorFunc& f)
{
    m_attr_op_functors[s] = f;
    logger().debug(
        "Added op functor \"{}\" among {} available",
        s,
        known_attribute_update_functors());
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
    wmtk::logger().debug("Creating a {} operation named {}", js["type"].get<std::string>(), name);
    std::string type = js["type"];
    try {
        const auto& f = m_op_functors.at(type);
        auto r = f(config, js);
        assert(bool(r));
        m_ops[std::string(name)] = {r, js};
        return r;
    } catch (const std::exception& e) {
        logger().error(
            "Was unable to create op functor \"{}\" among {} available: {}",
            type,
            known_operation_functors(),
            e.what());
        logger().error("Json was \n{}", js.dump(2));
        throw e;
    }
}
std::shared_ptr<wmtk::operations::Operation> OperationFactory::get(std::string_view name)
{
    return m_ops.at(std::string(name)).first;
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
        if (&op == my_op.first.get()) {
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
        auto& opt = opts.emplace_back() = op.second;


        // updates the invariant info to a normalized form
        for (const auto& inv : op.first->invariants().invariants()) {
            std::string inv_name = std::string(c.get_invariant_name(*inv));
            invariants::InvariantOptions iopt(
                "alias",
                invariants::AliasInvariantParameters(inv_name));
            opt.invariants[inv_name] = iopt;
        }
    }
    return opts;
}

auto OperationFactory::create_attribute_update_function(
    const Configurator& config,
    std::string_view name,
    const nlohmann::json& js) const -> AttributeUpdateFunction
{
    try {
        return m_attr_op_functors.at(std::string(name))(config, js);
    } catch (std::out_of_range& err) {
        logger().error(
            "Could not find attr update function \"{}\" among [{}]. Json was {}",
            name,
            known_attribute_update_functors(),
            js.dump());
        throw err;
    }
}

} // namespace wmtk::components::configurator::operations
