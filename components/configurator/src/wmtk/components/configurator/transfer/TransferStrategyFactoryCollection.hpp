#pragma once
#include <nlohmann/json_fwd.hpp>
#include <vector>
#include "TransferStrategyFactory.hpp"

namespace wmtk::components::configurator::transfer {

    // TODO: make this work without the inventory within the class
class TransferStrategyFactoryCollection: public 
    std::vector<std::shared_ptr<configurator::transfer::TransferStrategyFactory>> {};

class NamedTransferStrategyFactoryCollection: public 
    std::map<std::string,std::shared_ptr<configurator::transfer::TransferStrategyFactory>> {};


void to_json(nlohmann::json& j, const TransferStrategyFactoryCollection& t);
void from_json(const nlohmann::json& j, TransferStrategyFactoryCollection& t);
void to_json(nlohmann::json& j, const NamedTransferStrategyFactoryCollection& t);
void from_json(const nlohmann::json& j, NamedTransferStrategyFactoryCollection& t);
} // namespace wmtk::components::configurator::transfer

