
add_component_test(wmtk::${COMPONENT_NAME} 
    named_multimesh.cpp 
    get_attributes.cpp
    utils.hpp
    utils.cpp

    json.cpp

    utils/concatenate.cpp

    vertex_fusion.cpp

    from_manifold_decomposition.cpp
)

target_link_libraries(${WMTK_COMPONENT_TEST_TARGET} PRIVATE wmtk::input)
