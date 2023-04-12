include(catch2)
FetchContent_GetProperties(catch2)
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/contrib)
include(Catch)

file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "*.h" "*.cpp")

set(TEST_SOURCES
    Models.cpp
    main.cpp
    test_model_loader.cpp
    checks.cpp
    )

add_executable(extreme_opt_tests ${TEST_SOURCES})
target_link_libraries(extreme_opt_tests PUBLIC wmtk::extreme_opt Catch2::Catch2)
wmtk_copy_dll(extreme_opt_tests)

set_target_properties(extreme_opt_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

catch_discover_tests(extreme_opt_tests)
