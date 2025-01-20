#include "EdgeLengthTransferStrategyParameters.hpp"
#include "TransferStrategyOptions.hpp"

namespace wmtk::components::isotropic_remeshing::transfer {
void init()
{
    TransferStrategyOptions::register_transfer<EdgeLengthTransferStrategyParameters>("edge_length");
}
} // namespace wmtk::components::isotropic_remeshing::transfer
