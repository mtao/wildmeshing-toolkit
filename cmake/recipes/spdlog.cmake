#
# Copyright 2020 Adobe. All rights reserved.
# This file is licensed to you under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License. You may obtain a copy
# of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software distributed under
# the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR REPRESENTATIONS
# OF ANY KIND, either express or implied. See the License for the specific language
# governing permissions and limitations under the License.
#

# MIT

if(TARGET spdlog::spdlog)
    return()
endif()

include(fmt)
# the toolkit requires this setting now, ideally we could make this more optional, but it requires more care in making deps private
set(SPDLOG_FMT_EXTERNAL CACHE BOOL "Use external fmt library instead of bundled; this is forced by wmtk" ON)

# option(SPDLOG_FMT_EXTERNAL "Use external fmt library instead of bundled" ON)

message(STATUS "Third-party (external): creating target 'spdlog::spdlog'")

option(SPDLOG_INSTALL "Generate the install target" ON)


set(CMAKE_INSTALL_DEFAULT_COMPONENT_NAME "spdlog")

include(CPM)
CPMAddPackage("gh:gabime/spdlog@1.14.1")

# because we force the use of external FMT we want to make sure any dependency that uses spdlog has fmt around
target_link_libraries(spdlog PUBLIC fmt)

set_target_properties(spdlog PROPERTIES POSITION_INDEPENDENT_CODE ON)

set_target_properties(spdlog PROPERTIES FOLDER third_party)

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang" OR
   "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_compile_options(spdlog PRIVATE
        "-Wno-sign-conversion"
    )
endif()
