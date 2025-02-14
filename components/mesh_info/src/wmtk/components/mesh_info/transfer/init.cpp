#include "TransferStrategyOptions.hpp"
#include "edge_length.hpp"
#include "mean_neighbor.hpp"
#include "mean_ratio_measure.hpp"
#include "min_neighbor.hpp"

namespace wmtk::components::mesh_info::transfer {
void init()
{
    TransferStrategyOptions::register_transfer<EdgeLength>("edge_length");
    TransferStrategyOptions::register_transfer<MeanNeighbor>("mean");
    TransferStrategyOptions::register_transfer<MinNeighbor>("min");
    TransferStrategyOptions::register_transfer<MeanRatioMeasure>("mean_ratio_measure");
    TransferStrategyOptions::register_transfer<MeanNeighbor>("mean_neighbor");
    TransferStrategyOptions::register_transfer<MinNeighbor>("min_neighbor");
}
} // namespace wmtk::components::mesh_info::transfer
