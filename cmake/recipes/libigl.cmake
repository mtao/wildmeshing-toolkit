if(TARGET igl::core)
    return()
endif()

message(STATUS "Third-party: creating target 'igl::core'")

include(FetchContent)
CPMAddPackage(
    libigl
    GIT_REPOSITORY https://github.com/libigl/libigl.git
    GIT_TAG v2.6.0
    OPTIONS LIBIGL_INSTALL OFF
)
# include(eigen)
FetchContent_MakeAvailable(libigl)
