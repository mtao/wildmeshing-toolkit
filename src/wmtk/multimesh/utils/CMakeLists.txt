
set(SRC_FILES
    find_local_switch_sequence.cpp
    find_local_switch_sequence.hpp
    find_local_dart_action.cpp
    find_local_dart_action.hpp
    local_switch_tuple.cpp
    local_switch_tuple.hpp
    transport_tuple.cpp
    transport_tuple.hpp

    transfer_attribute.hpp
    transfer_attribute.cpp

    tuple_map_attribute_io.hpp
    tuple_map_attribute_io.cpp
   
    internal/TupleTag.hpp
    internal/TupleTag.cpp

    internal/print_all_mapped_tuples.hpp
    internal/print_all_mapped_tuples.cpp


    create_tag.hpp
    create_tag.cpp
    extract_child_mesh_from_tag.hpp
    extract_child_mesh_from_tag.cpp

    extract_child_simplices_and_map_from_tag.hpp
    extract_child_simplices_and_map_from_tag.cpp


    check_map_valid.cpp
    check_map_valid.hpp

    MapValidator.hpp
    MapValidator.cpp

    get_points.cpp
    get_points.hpp
    )
target_sources(wildmeshing_toolkit PRIVATE ${SRC_FILES})
