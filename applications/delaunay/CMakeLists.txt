wmtk_add_application(delaunay_app
    delaunay_main.cpp
    delaunay_spec.hpp
    )




# delaunay requires the input component and the delaunay component
target_link_libraries(delaunay_app PRIVATE
wmtk::input
wmtk::delaunay
wmtk::output
wmtk::to_points
wmtk::application_utils
)

wmtk_register_integration_test(
    EXEC_NAME delaunay_app
    CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/delaunay_test_config.json
    GIT_REPOSITORY "https://github.com/wildmeshing/data.git"
    GIT_TAG 152a561697a6e923451ca8535309cbe1e116a9fa)
