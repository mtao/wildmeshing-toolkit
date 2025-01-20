#pragma once
#include <functional>
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/utils/json_macros.hpp>
namespace wmtk {
namespace operations {
class AttributeTransferStrategyBase;
}
namespace components::multimesh {
class MeshCollection;
}
} // namespace wmtk
namespace wmtk::components::isotropic_remeshing::transfer {
struct TransferStrategyParameters;
struct TransferStrategyOptions
{
    TransferStrategyOptions();
    ~TransferStrategyOptions();
    TransferStrategyOptions(const TransferStrategyOptions&);
    TransferStrategyOptions(TransferStrategyOptions&&);
    TransferStrategyOptions& operator=(const TransferStrategyOptions&);
    TransferStrategyOptions& operator=(TransferStrategyOptions&&);
    std::string attribute_path;
    std::string type;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(TransferStrategyOptions)
    std::unique_ptr<TransferStrategyParameters> parameters;
    std::shared_ptr<wmtk::operations::AttributeTransferStrategyBase> create(
        wmtk::components::multimesh::MeshCollection& mc) const;

    friend struct TransferStrategyParameters;
    friend void init();

private:
    static std::map<
        std::string,
        std::function<std::unique_ptr<TransferStrategyParameters>(const nlohmann::json&)>>
        s_registered_transfers;

    template <typename Type>
    void static register_transfer(const std::string& name);
};

template <typename Type>
void TransferStrategyOptions::register_transfer(const std::string& name)
{
    if (s_registered_transfers.find(name) == s_registered_transfers.end()) {
        s_registered_transfers.emplace(name, [](const nlohmann::json& js) {
            auto t = std::make_unique<Type>();
            t->from_json(js);
            return t;
        });
    }
}
} // namespace wmtk::components::isotropic_remeshing::transfer
