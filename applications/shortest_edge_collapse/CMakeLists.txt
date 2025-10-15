include(wmtk_add_application)
wmtk_add_application(shortest_edge_collapse_app
    main.cpp
    spec.hpp
    )


target_link_libraries(shortest_edge_collapse_app PRIVATE
wmtk::input
wmtk::shortest_edge_collapse
wmtk::multimesh
wmtk::output
wmtk::application_utils
nlohmann_json::nlohmann_json
)

wmtk_register_integration_test(
    EXEC_NAME shortest_edge_collapse_app
    CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/shortest_edge_collapse_test_config.json
)
