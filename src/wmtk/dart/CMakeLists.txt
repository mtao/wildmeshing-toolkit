

set(SRC_FILES


    local_dart_action.hpp
    local_dart_action.cpp

    SimplexDart.hpp
    SimplexDart.cpp

    Dart.hpp
    Dart.cpp

    tuple_from_valid_index.hpp
    tuple_from_valid_index.cpp

    valid_index_from_tuple.hpp
    valid_index_from_tuple.cpp

    simplex_index_from_valid_index.hpp
    simplex_index_from_valid_index.cpp

    subgroup/convert.hpp


    SimplexAdjacency.hpp
)

target_sources(wildmeshing_toolkit PRIVATE ${SRC_FILES})
add_subdirectory(utils)
