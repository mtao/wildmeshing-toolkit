## libigl MPL

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
if(TARGET igl::core)
    return()
endif()

message(STATUS "Third-party: creating target 'igl::core'")

include(FetchContent)
FetchContent_Declare(
    libigl
    #GIT_REPOSITORY https://github.com/libigl/libigl.git
    #GIT_TAG 3ea7f9480967fcf6bf02ce9b993c0ea6d2fc45f6
    GIT_REPOSITORY https://github.com/mtao/libigl.git
    #GIT_TAG mtao/warning_suppression
    GIT_TAG ebdac2554f7d92143b98e1694cac603b4839348e 
)
include(eigen)
option(LIBIGL_INSTALL "Enable installation of libigl targets" ON)
FetchContent_MakeAvailable(libigl)
