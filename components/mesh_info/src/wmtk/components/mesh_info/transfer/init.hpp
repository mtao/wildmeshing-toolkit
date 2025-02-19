#pragma once
#include "TransferStrategyFactoryRegistry.hpp"

namespace wmtk::components::mesh_info::transfer {
void init();
TransferStrategyFactoryRegistry create_registry();
}
