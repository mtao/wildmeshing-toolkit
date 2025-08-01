add_component(${COMPONENT_NAME})
if(NOT ${WMTK_ENABLE_COMPONENT_${COMPONENT_NAME}})
    return()
endif()
set(SRC_FILES
)


target_compile_features(wmtk_${COMPONENT_NAME} PUBLIC cxx_std_20)


target_sources(wmtk_${COMPONENT_NAME} PRIVATE ${SRC_FILES})
target_link_libraries(wmtk_${COMPONENT_NAME} PRIVATE nlohmann_json)
target_link_libraries(wmtk_${COMPONENT_NAME} PUBLIC wmtk::multimesh)

add_subdirectory(transfer)
add_subdirectory(operations)
#add_subdirectory(invariants)
