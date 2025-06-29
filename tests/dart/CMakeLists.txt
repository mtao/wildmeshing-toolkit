
# Sources
set(TEST_SOURCES
    dart.cpp
    group_structure.cpp
    actions.cpp
    simplex_adjacency.cpp
    dart_accessor.cpp
    topology_initialization.cpp
    edge_mirror.cpp
    relative_faces.cpp

    simplex_involution.cpp

    relative_permutation.cpp

    utils/canonical_darts.hpp
    utils/canonical_darts.cpp
)
target_sources(wmtk_tests PRIVATE ${TEST_SOURCES})
