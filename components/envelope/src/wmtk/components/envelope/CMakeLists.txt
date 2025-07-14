if(NOT ${WMTK_ENABLE_COMPONENT_${COMPONENT_NAME}})
    return()
endif()
add_component(${COMPONENT_NAME})
set(SRC_FILES
    EnvelopeOptions.hpp
    EnvelopeOptions.cpp
)

include(stb)


target_compile_features(wmtk_${COMPONENT_NAME} PUBLIC cxx_std_20)

target_sources(wmtk_${COMPONENT_NAME} PRIVATE ${SRC_FILES})
target_link_libraries(wmtk_${COMPONENT_NAME} PUBLIC nlohmann_json wmtk::multimesh)

