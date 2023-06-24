#pragma once


namespace wmtk {


class Scheduler 
{
    using Op = std::pair<std::string, Tuple>;
    template <typename OperationType, typename... Args>
    void add_operation_type(const std::string& name, PrimitiveType primitive_type, Args... args)
    {
        m_factories[name] = std::make_unique<OperationFactory<OperationType>>(primitive_type, std::forward<Args>(args)...);
    }

    void run_queue(const std::vector<Op>& tuples)
    {
        while (!empty()) {
            auto new_op = get_operation(pop_top());
            auto new_op = get_factory(name)->create(m, tup);
            new_op->run();
        }
    }
    std::unique_ptr<Operation> get_operation(const Op& pr)
    {
        const auto& [name, tup] = pr;
        return get_factory(name)->create(m, tup);
    }


private:
    Mesh& m;
    std::unordered_map<std::string, std::unique_ptr<OperationFactory>> m_factories;
};

} // namespace wmtk