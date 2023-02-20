
################################################################################
# extreme opt
################################################################################

include(gmp)
include(fenvelope)

if(NOT WMTK_APP_UNIFORM_REMESH) 
    message(FATAL_ERROR "ExtremeOpt requires uniform remeshing; try WMTK_APP_UNIFORM_REMESH=ON")
endif()

add_library(wmtk_extreme_opt
	opt.cpp
	ExtremeOpt.cpp
	ExtremeOpt.h
	SYMDIR.cpp
	SYMDIR.h
)
add_library(wmtk::extreme_opt ALIAS wmtk_extreme_opt)

target_include_directories(wmtk_extreme_opt PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/)

target_link_libraries(wmtk_extreme_opt PUBLIC
	Tracy::TracyClient
	wmtk::toolkit
	wmtk::data
	# wmtk::sec_lib
	wmtk::remeshing_lib
	igl::predicates
	gmp::gmp
)

add_executable(extreme_opt main.cpp)
target_compile_options(extreme_opt PRIVATE "-rdynamic")
target_link_libraries(extreme_opt PUBLIC wmtk::extreme_opt)
wmtk_copy_dll(extreme_opt)

# add_executable(viewer viewer.cpp)
# target_link_libraries(viewer igl::core igl::opengl igl::opengl_glfw)

add_executable(test_json test_json.cpp)
