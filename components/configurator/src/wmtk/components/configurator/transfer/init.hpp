#pragma once
#include "TransferStrategyFactoryRegistry.hpp"

namespace wmtk::components::configurator::transfer {
void init();
// TODO: one should generally just use the ptr version, but the validity of this should be verified
TransferStrategyFactoryRegistry create_registry();
std::shared_ptr<TransferStrategyFactoryRegistry> create_registry_ptr();
} // namespace wmtk::components::configurator::transfer
