include(wmtk_add_application)
include(wmtk_register_jse_json)
wmtk_add_application(mesh_info_app
    main.cpp
    )

target_compile_features(mesh_info_app PUBLIC cxx_std_20)
    #register_jse_json(APPLICATION_NAME mesh_info INPUT mesh_info_spec.json )

# mesh_info requires the output component and the mesh_info component
target_link_libraries(mesh_info_app PRIVATE
wmtk::input
wmtk::output
wmtk::mesh_info
wmtk::multimesh
wmtk::application_utils
)

wmtk_register_integration_test(
    EXEC_NAME mesh_info_app
    CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test_config.json
    CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}/examples
    EXTRA_ARGUMENTS json
)
