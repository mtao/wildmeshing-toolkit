#include "TransferStrategyFactory.hpp"
#include "edge_angle.hpp"
#include "edge_length.hpp"
#include "max_neighbor.hpp"
#include "mean_neighbor.hpp"
#include "mean_ratio_measure.hpp"
#include "min_neighbor.hpp"
#include "normal.hpp"
#include "threshold.hpp"
#include "volume.hpp"
#include "neighbor_count.hpp"

namespace wmtk::components::mesh_info::transfer {
TransferStrategyFactoryRegistry create_registry()
{
    spdlog::info("Registering!");
    TransferStrategyFactoryRegistry reg;
    reg.register_transfer<EdgeLength>("edge_length");
    reg.register_transfer<MeanNeighbor>("mean");
    reg.register_transfer<MinNeighbor>("min");
    reg.register_transfer<MaxNeighbor>("max");
    reg.register_transfer<MeanRatioMeasure>("mean_ratio_measure");
    reg.register_transfer<MeanNeighbor>("mean_neighbor");
    reg.register_transfer<MinNeighbor>("min_neighbor");
    reg.register_transfer<MaxNeighbor>("max_neighbor");
    reg.register_transfer<Normal>("normal");
    reg.register_transfer<EdgeAngle>("edge_angle");
    reg.register_transfer<Volume>("volume");
    reg.register_transfer<Threshold>("threshold");
    reg.register_transfer<NeighborCount>("neighbor_count");
    return reg;
}
void init()
{
    TransferStrategyFactory::s_transfer_registry = create_registry();
}
} // namespace wmtk::components::mesh_info::transfer
