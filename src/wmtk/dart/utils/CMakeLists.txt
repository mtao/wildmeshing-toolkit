
set(SRC_FILES
    relative_face_index.cpp
    relative_face_index.hpp

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


    edge_mirror.hpp
    edge_mirror.cpp

    get_simplex_involution.hpp
    get_simplex_involution.cpp

    tuple_from_permutation_index.hpp
    tuple_from_permutation_index.cpp

    permutation_index_from_tuple.hpp
    permutation_index_from_tuple.cpp

    get_simplex_dart.hpp
    )

target_sources(wildmeshing_toolkit PRIVATE ${SRC_FILES})
