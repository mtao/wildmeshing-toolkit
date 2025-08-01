#pragma once
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include "TransferStrategyFactory.hpp"

namespace wmtk::components::configurator::transfer {

class TransferStrategyFactoryCollection: public 
    std::vector<std::shared_ptr<configurator::transfer::TransferStrategyFactory>> {};


void to_json(nlohmann::json& j, const TransferStrategyFactoryCollection& t);
void from_json(const nlohmann::json& j, TransferStrategyFactoryCollection& t);
} // namespace wmtk::components::configurator::transfer

