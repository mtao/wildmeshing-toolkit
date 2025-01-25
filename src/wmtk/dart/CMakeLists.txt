

set(SRC_FILES


    local_dart_action.hpp
    local_dart_action.cpp

    SimplexDart.hpp
    SimplexDart.cpp

    Dart.hpp
    Dart.cpp



    subgroup/convert.hpp


    SimplexAdjacency.hpp
)

target_sources(wildmeshing_toolkit PRIVATE ${SRC_FILES})
add_subdirectory(utils)
