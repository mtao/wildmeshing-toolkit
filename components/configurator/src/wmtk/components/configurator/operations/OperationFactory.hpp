#pragma once
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>
#include <wmtk/operations/AttributeUpdate.hpp>
#include "OperationOptions.hpp"


namespace wmtk::operations {
class Operation;
class AttributeUpdate;
} // namespace wmtk::operations
namespace wmtk::components::multimesh {
class MeshCollection;
}

namespace wmtk::components::configurator {
class Configurator;
}

namespace wmtk::components::configurator::operations {


class OperationFactory
{
public:
    OperationFactory();
    using OpCreatorFunc = std::function<
        std::shared_ptr<wmtk::operations::Operation>(Configurator&, const nlohmann::json& js)>;
    using AttrUpdateOpCreatorFunc = std::function<
        // std::shared_ptr<wmtk::operations::AttributeUpdate>
        wmtk::operations::AttributeUpdateWithFunction::UpdateFunction(
            Configurator&,
            const nlohmann::json& js)>;
    // using AttrUpdateOpCreatorFunc = std::function<
    //     std::shared_ptr<wmtk::operations::AttributeUpdate>(Configurator&, const nlohmann::json&
    //     js)>;

    void add(const std::string& s, const OpCreatorFunc& f);
    void add_attribute_function(const std::string& s, const AttrUpdateOpCreatorFunc& f);

    void from_json(Configurator&, const nlohmann::json& js);

    std::shared_ptr<wmtk::operations::Operation> create(
        Configurator& config,
        const nlohmann::json& js);
    std::shared_ptr<wmtk::operations::Operation>
    create(Configurator& config, std::string_view name, const nlohmann::json& js);
    std::shared_ptr<wmtk::operations::Operation> get(std::string_view name);

    std::string_view get_name(const wmtk::operations::Operation& op) const;

    std::vector<OperationOptions> get_options(const Configurator& o) const;

    std::vector<std::string> known_operation_functors() const;
    std::vector<std::string> known_attribute_update_functors() const;
    std::vector<std::string> known_operations() const;

private:
    std::map<std::string, OpCreatorFunc> m_op_functors;
    std::map<std::string, AttrUpdateOpCreatorFunc> m_attr_op_functors;

    std::map<std::string, std::pair<std::shared_ptr<wmtk::operations::Operation>, nlohmann::json>>
        m_ops;
};
} // namespace wmtk::components::configurator::operations
