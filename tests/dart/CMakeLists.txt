
# Sources
set(TEST_SOURCES
    dart.cpp
    actions.cpp
    simplex_adjacency.cpp
    dart_accessor.cpp
    topology_initialization.cpp
    relative_faces.cpp

    utils/canonical_darts.hpp
    utils/canonical_darts.cpp
)
target_sources(wmtk_tests PRIVATE ${TEST_SOURCES})
