# Copyright 2022 Google LLC
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Usage:
# cd .../AndroidStudioProjects/OthelloSensei/app
# cmake -S src/main/cpp -B build -DANDROID=FALSE && cmake --build build && ./build/FILE

# Profile:
# cmake -S src/main/cpp -B build -DANDROID=FALSE -DPROFILE=TRUE && cmake --build build
# valgrind --tool=callgrind --log_file=/tmp/valgrind ./build/evaluator_last_moves_profile
# kcachegrind callgrind.out.XXXXX

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("othellosensei" CXX)

IF(NOT DEFINED ANDROID)
    SET(ANDROID TRUE)
ENDIF()
IF(NOT DEFINED PROFILE)
    SET(PROFILE FALSE)
ENDIF()

add_subdirectory(analyzers)
add_subdirectory(board)
add_subdirectory(evaluatedepthone)
add_subdirectory(evaluateindepth)
add_subdirectory(hashmap)
add_subdirectory(utils)

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")

enable_testing()

IF(ANDROID)
    find_library(log-lib JNI_LIBRARIES)
ELSE()
    find_package(GTest)
    find_package(JNI REQUIRED)
    include_directories( ${JNI_INCLUDE_DIRS} )
ENDIF()

SET(PDEP_PEXT NOT ANDROID)

IF(${PDEP_PEXT})
    add_definitions(-DPDEP_PEXT=true)
ELSE()
    add_definitions(-DPDEP_PEXT=false)
ENDIF()

SET(
        RELEASE_FLAGS
        -pedantic
        -pipe
        -D_GNU_SOURCE=1
        -Ofast
        -DNDEBUG
        -DUSE_GAS_X64
        -DPOPCOUNT
        -DHAS_CPU_64
)

IF(NOT ANDROID)
    SET(
          RELEASE_FLAGS
          ${RELEASE_FLAGS}
          -m64
          -march=native
    )
ENDIF()
IF (PROFILE)
    SET(
            RELEASE_FLAGS
            ${RELEASE_FLAGS}
            -g
    )
ENDIF()

add_library(
        constants
        constants.h
)

set_target_properties(
        constants PROPERTIES LINKER_LANGUAGE CXX
)
