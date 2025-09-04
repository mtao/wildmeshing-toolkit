

if(TARGET perfetto)
    return()
endif()

message(STATUS "Third-party (external): creating target 'perfetto'")

include(CPM)
CPMAddPackage(
    NAME perfetto 
    GITHUB_REPOSITORY google/perfetto
    GIT_TAG v50.1
)

set(PERFETTO_SDK_DIR "${perfetto_SOURCE_DIR}/sdk")
add_library(perfetto "${PERFETTO_SDK_DIR}/perfetto.cc")
target_include_directories(perfetto PUBLIC "${PERFETTO_SDK_DIR}")

set_target_properties(perfetto PROPERTIES FOLDER third_party)

