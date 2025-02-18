#include "TransferStrategyFactory.hpp"
#include "edge_length.hpp"
#include "max_neighbor.hpp"
#include "mean_neighbor.hpp"
#include "mean_ratio_measure.hpp"
#include "min_neighbor.hpp"
#include "normal.hpp"

namespace wmtk::components::mesh_info::transfer {
void init()
{
    TransferStrategyFactory::register_transfer<EdgeLength>("edge_length");
    TransferStrategyFactory::register_transfer<MeanNeighbor>("mean");
    TransferStrategyFactory::register_transfer<MinNeighbor>("min");
    TransferStrategyFactory::register_transfer<MaxNeighbor>("max");
    TransferStrategyFactory::register_transfer<MeanRatioMeasure>("mean_ratio_measure");
    TransferStrategyFactory::register_transfer<MeanNeighbor>("mean_neighbor");
    TransferStrategyFactory::register_transfer<MinNeighbor>("min_neighbor");
    TransferStrategyFactory::register_transfer<MaxNeighbor>("max_neighbor");
    TransferStrategyFactory::register_transfer<Normal>("normal");
    // TransferStrategyFactory::register_transfer<EdgeAngle>("edge_angle");
}
} // namespace wmtk::components::mesh_info::transfer
