#include "TransferStrategyFactory.hpp"
#include "amips.hpp"
#include "edge_angle.hpp"
#include "edge_length.hpp"
#include "max_neighbor.hpp"
#include "mean_neighbor.hpp"
#include "mean_ratio_measure.hpp"
#include "min_neighbor.hpp"
#include "neighbor_count.hpp"
#include "normal.hpp"
#include "threshold.hpp"
#include "volume.hpp"

namespace wmtk::components::configurator::transfer {
TransferStrategyFactoryRegistry create_registry()
{
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
    reg.register_transfer<AMIPS>("amips");
    return reg;
}
void init()
{
    TransferStrategyFactory::s_transfer_registry = create_registry();
}
} // namespace wmtk::components::configurator::transfer
