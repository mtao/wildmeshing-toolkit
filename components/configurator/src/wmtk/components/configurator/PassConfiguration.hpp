
#pragma once
#include <memory>
#include <nlohmann/json_fwd.hpp>
#include <wmtk/components/input/InputOptions.hpp>
#include <wmtk/components/output/OutputOptions.hpp>
#include "PassOptions.hpp"
#include "invariants/InvariantOptions.hpp"
#include "operations/OperationOptions.hpp"
#include "transfer/TransferStrategyFactoryCollection.hpp"


namespace wmtk::components::configurator {
struct PassConfiguration
{
    // defines the operations that will be used in a pass. The contents should include a "type"
    // which
    std::map<std::string, operations::OperationOptions> operations;
    // defines the invariants used in the aforementioned invariants. they will be referred to in the
    // above section
    std::map<std::string, invariants::InvariantOptions> invariants;

    transfer::TransferStrategyFactoryCollection transfers;
    // std::vector<std::shared_ptr<operations::OperationOptions>> operations;
    // std::vector<std::shared_ptr<invariants::InvariantOptions>> invariants;

    // operations::OperationOptions get_operation(std::string_view name);
    // invariants::InvariantOptions get_invariant(std::string_view name);
    // operations::OperationOptions set_operation(std::string_view name);
    // invariants::InvariantOptions set_invariant(std::string_view name);
    std::vector<PassOptions> passes;
    WMTK_NLOHMANN_JSON_FRIEND_DECLARATION(PassConfiguration);
};
} // namespace wmtk::components::configurator
