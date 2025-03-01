macro(wmtk_register_integration_test )

 set(options )
 set(oneValueArgs EXEC_NAME CONFIG_FILE CONFIG_PATH EXTRA_ARGUMENTS)
 set(multiValueArgs)
 cmake_parse_arguments("" "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )
    include(CPM)


    MESSAGE(STATUS "Registering integration test for ${_EXEC_NAME}")
    file(READ ${_CONFIG_FILE} MY_JSON_STRING)
    string(JSON TEST_JSONS GET "${MY_JSON_STRING}" "tests" )
    string(JSON TEST_JSONS_LENGTH LENGTH "${TEST_JSONS}" )
    set(APP_TESTS "")
    if(${TEST_JSONS_LENGTH} GREATER 0)
        math(EXPR TEST_JSONS_LENGTH "${TEST_JSONS_LENGTH} - 1")
        foreach(INDEX RANGE 0 ${TEST_JSONS_LENGTH})
            string(JSON TEST_NAME GET "${TEST_JSONS}" ${INDEX})
            list(APPEND APP_TESTS ${TEST_NAME})
        endforeach()
    endif()




    include(wmtk_download_data)
    wmtk_download_data()
    set(DATA_DIR_LINE  "\"data_folder\":\"${DATA_DIR}\"," )

    if(NOT _CONFIG_PATH)
        set(_CONFIG_PATH ${DATA_DIR})
    endif()

    list (APPEND WMTK_APPLICATION_TEST_NAMES "${_EXEC_NAME}")
    list (APPEND WMTK_TEST_CONFIG 
        "\"${_EXEC_NAME}\":
        {
        ${DATA_DIR_LINE}
        \"config_file\":\"${_CONFIG_FILE}\",
        \"config_folder\":\"${_CONFIG_PATH}\",
        \"extra_flags\":\"${_EXTRA_ARGUMENTS}\"
        }")

        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/.." APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_CONFIG_PATH}")

    SET(WMTK_TEST_CONFIG ${WMTK_TEST_CONFIG} PARENT_SCOPE)
    SET(WMTK_APPLICATION_TEST_NAMES ${WMTK_APPLICATION_TEST_NAMES} PARENT_SCOPE)
    SET(TEST_NAMES "WMTK_APPLICATION_TEST_${_EXEC_NAME}_CONFIGS")
    SET(${TEST_NAMES} ${APP_TESTS} PARENT_SCOPE)
endmacro()
