
set(SRC_FILES
    same_simplex_dimension_surjection.hpp
    same_simplex_dimension_surjection.cpp
    same_simplex_dimension_bijection.hpp
    same_simplex_dimension_bijection.cpp
    from_facet_orientations.hpp
    from_facet_orientations.cpp
    MultiMeshSimplexVisitor.hpp

    MultiMeshManager.hpp
    MultiMeshManager.cpp
    MultiMeshManager_map_updates.cpp
    BoundaryChecker.cpp
    BoundaryChecker.hpp
    consolidate.hpp
    consolidate.cpp
    )
target_sources(wildmeshing_toolkit PRIVATE ${SRC_FILES})

add_subdirectory(utils)
add_subdirectory(attribute)
add_subdirectory(operations)
