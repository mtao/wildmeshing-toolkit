
#include <numeric>

#include <spdlog/stopwatch.h>
#include <catch2/catch_test_macros.hpp>
#include <wmtk/attribute/TupleAccessor.hpp>
#include <wmtk/dart/DartTopologyAccessor.hpp>
#include <wmtk/dart/utils/indexed_topology_initialization.hpp>
#include <wmtk/io/read_mesh.hpp>
#include <wmtk/multimesh/utils/tuple_map_attribute_io.hpp>
#include <wmtk/utils/Logger.hpp>
#include "tools/TriMesh_examples.hpp"

TEST_CASE("dart_topology_initialization", "[dart_accessor][.]")

{
    auto mesh = wmtk::tests::three_neighbors();

    auto handle = wmtk::dart::register_dart_boundary_topology_attribute(mesh, "dart", true);


    wmtk::dart::DartTopologyAccessor acc(mesh, handle);

    //auto FV = wmtk::dart::utils::indexed_topology_initialization<0>(acc);
    auto FE = wmtk::dart::utils::indexed_topology_initialization<1>(acc);

    //std::cout << FV << std::endl << std::endl;
    std::cout << FE << std::endl << std::endl;
}

