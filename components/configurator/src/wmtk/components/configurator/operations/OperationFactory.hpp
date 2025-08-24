#pragma once
#include <functional>
#include <memory>
#include <nlohmann/json_fwd.hpp>


namespace wmtk::operations {
class Operation;
}
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

    void add(const std::string& s, const OpCreatorFunc& f);

    void from_json(Configurator&, const nlohmann::json& js);

    std::shared_ptr<wmtk::operations::Operation> create(
        Configurator& config,
        const nlohmann::json& js);
    std::shared_ptr<wmtk::operations::Operation>
    create(Configurator& config, std::string_view name, const nlohmann::json& js);
    std::shared_ptr<wmtk::operations::Operation> get(std::string_view name);

    std::string_view get_name(const wmtk::operations::Operation& op) const;


    std::vector<std::string> known_operation_functors() const;
    std::vector<std::string> known_operations() const;

private:
    std::map<std::string, OpCreatorFunc> m_op_functors;

    std::map<std::string, std::shared_ptr<wmtk::operations::Operation>> m_ops;
};
} // namespace wmtk::components::configurator::operations
