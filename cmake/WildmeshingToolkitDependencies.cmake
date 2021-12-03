# Prepare dependencies
#
# For each third-party library, if the appropriate target doesn't exist yet,
# download it via external project, and add_subdirectory to build it alongside
# this project.

### Configuration
set(WILDMESHING_TOOLKIT_ROOT     "${CMAKE_CURRENT_LIST_DIR}/..")

# Download and update 3rdparty libraries
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})
list(REMOVE_DUPLICATES CMAKE_MODULE_PATH)
include(WildmeshingToolkitDownloadExternal)

################################################################################
# Required libraries
################################################################################

# Sanitizers
if(WILDMESHING_TOOLKIT_WITH_SANITIZERS)
    wildmeshing_toolkit_download_sanitizers()
    find_package(Sanitizers)
endif()

# spdlog
if(NOT TARGET spdlog::spdlog)
    wildmeshing_toolkit_download_spdlog()
    add_subdirectory(${WILDMESHING_TOOLKIT_EXTERNAL}/spdlog)
endif()


# igl
if(NOT TARGET igl::core)
    wildmeshing_toolkit_download_libigl()
    add_subdirectory(${WILDMESHING_TOOLKIT_EXTERNAL}/libigl)
endif()