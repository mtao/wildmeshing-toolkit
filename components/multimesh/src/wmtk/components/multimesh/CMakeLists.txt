set(COMPONENT_NAME multimesh)
add_component(${COMPONENT_NAME})
if(NOT ${WMTK_ENABLE_COMPONENT_${COMPONENT_NAME}})
    return()
endif()
set(SRC_FILES
    axis_aligned_fusion.hpp
    axis_aligned_fusion.cpp

    from_facet_bijection.hpp
    from_facet_bijection.cpp


    from_boundary.hpp
    from_boundary.cpp

    from_tag.hpp
    from_tag.cpp


    multimesh.hpp
    multimesh.cpp
)

include(stb)


target_compile_features(wmtk_${COMPONENT_NAME} PUBLIC cxx_std_20)

target_sources(wmtk_${COMPONENT_NAME} PRIVATE ${SRC_FILES})
