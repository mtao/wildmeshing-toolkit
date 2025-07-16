
set(SRC_FILES
    relative_face_index.cpp
    relative_face_index.hpp

    local_simplex_decomposition.hpp
    local_simplex_decomposition.cpp

    simplex_index_from_permutation_index.cpp
    simplex_index_from_permutation_index.hpp

    permutation_index_to_simplex_index.cpp
    permutation_index_to_simplex_index.hpp

    subdart_maximal_action_to_face.hpp
    subdart_maximal_action_to_face.cpp

    largest_shared_subdart_size.hpp
    largest_shared_subdart_size.cpp

    tuple_from_permutation_index.hpp
    tuple_from_permutation_index.cpp

    get_faces.cpp
    get_faces.hpp
    get_canonical_faces.cpp
    get_canonical_faces.hpp

    get_canonical_cofaces.cpp
    get_canonical_cofaces.hpp
    
    opposite.hpp
    opposite.cpp

    edge_mirror.hpp
    edge_mirror.cpp

    get_simplex_involution.hpp
    get_simplex_involution.cpp

    apply_simplex_involution.hpp
    apply_simplex_involution.cpp

    tuple_from_permutation_index.hpp
    tuple_from_permutation_index.cpp

    permutation_index_from_tuple.hpp
    permutation_index_from_tuple.cpp

    get_simplex_dart.hpp

    get_local_vertex_permutation.hpp
    get_local_vertex_permutation.cpp

    from_local_vertex_permutation.hpp
    from_local_vertex_permutation.cpp
    )

target_sources(wildmeshing_toolkit PRIVATE ${SRC_FILES})
