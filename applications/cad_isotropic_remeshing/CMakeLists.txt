include(wmtk_add_application)
include(wmtk_register_jse_json)



wmtk_add_application(cad_isotropic_remeshing_app
    main.cpp
    Mesh.hpp
    Mesh.cpp
    EigenMeshes.hpp
    EigenMeshes.cpp
    )



# isotropic_remeshing requires the input component and the procedural component
target_link_libraries(cad_isotropic_remeshing_app PRIVATE
wmtk::input
wmtk::isotropic_remeshing
wmtk::multimesh
wmtk::output
wmtk::application_utils
)


#
#wmtk_register_integration_test(
#    EXEC_NAME isotropic_remeshing_uv_app
#    CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/test_config_uv.json
#    GIT_REPOSITORY "https://github.com/wildmeshing/data.git"
#    GIT_TAG 152a561697a6e923451ca8535309cbe1e116a9fa
#    CONFIG_PATH ${CMAKE_CURRENT_SOURCE_DIR}
#)
