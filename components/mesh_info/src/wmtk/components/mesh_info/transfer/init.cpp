#include "EdgeLengthTransferStrategyParameters.hpp"
#include "MeanRatioMeasureTransferStrategyParameters.hpp"
#include "MeanTransferStrategyParameters.hpp"
#include "MinTransferStrategyParameters.hpp"
#include "TransferStrategyOptions.hpp"

namespace wmtk::components::mesh_info::transfer {
void init()
{
    TransferStrategyOptions::register_transfer<EdgeLengthTransferStrategyParameters>("edge_length");
    TransferStrategyOptions::register_transfer<MeanTransferStrategyParameters>("mean");
    TransferStrategyOptions::register_transfer<MinTransferStrategyParameters>("min");
    TransferStrategyOptions::register_transfer<MeanRatioMeasureTransferStrategy>("mean_ratio_measure");
}
} // namespace wmtk::components::mesh_info::transfer
