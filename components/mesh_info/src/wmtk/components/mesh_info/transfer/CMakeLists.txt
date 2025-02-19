
target_sources(wmtk_${COMPONENT_NAME} PRIVATE 
TransferStrategyFactory.cpp
TransferStrategyFactory.hpp
TransferStrategy_macros.hpp

TransferStrategyFactoryRegistry.cpp
TransferStrategyFactoryRegistry.hpp

TransferStrategyFactoryCollection.cpp
TransferStrategyFactoryCollection.hpp

SingleAttributeTransferStrategyFactory.cpp
SingleAttributeTransferStrategyFactory.hpp

edge_length.cpp
edge_length.hpp

mean_ratio_measure.hpp
mean_ratio_measure.cpp

normal.cpp
normal.hpp

edge_angle.hpp
edge_angle.cpp

volume.hpp
volume.cpp

mean_neighbor.hpp
mean_neighbor.cpp
min_neighbor.hpp
min_neighbor.cpp
max_neighbor.hpp
max_neighbor.cpp

init.hpp
init.cpp
)
