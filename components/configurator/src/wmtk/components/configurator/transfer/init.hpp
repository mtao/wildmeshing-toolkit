#pragma once
#include "TransferStrategyFactoryRegistry.hpp"

namespace wmtk::components::configurator::transfer {
void init();
TransferStrategyFactoryRegistry create_registry();
}
