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
# cmake -S src/main/cpp -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo && \
# cmake --build build && \
# valgrind --tool=callgrind --callgrind-out-file=/tmp/callgrind.out ./build/analyzers/endgame_ffo && \
# kcachegrind /tmp/callgrind.out

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

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC -pthread")

enable_testing()

IF(ANDROID)
    find_library(log-lib JNI_LIBRARIES)
ELSE()
    find_package(GTest REQUIRED)
    find_package(JNI REQUIRED)
ENDIF()
include_directories( ${JNI_INCLUDE_DIRS} )

SET(PDEP_PEXT NOT ANDROID)

IF(${PDEP_PEXT})
    add_definitions(-DPDEP_PEXT=true)
ELSE()
    add_definitions(-DPDEP_PEXT=false)
ENDIF()

SET(
        CMAKE_CXX_FLAGS_RELEASE
        "-pedantic -pipe -D_GNU_SOURCE=1 -m64 -Ofast -DNDEBUG -march=native -funroll-loops -DUSE_GAS_X64 -DPOPCOUNT -DHAS_CPU_64 -fomit-frame-pointer -DUSE_GCC_ARM"
)

IF(NOT ANDROID)
    SET(
          CMAKE_CXX_FLAGS_RELEASE
          "${CMAKE_CXX_FLAGS_RELEASE}"
    )
ENDIF()
IF (PROFILE)
    SET(
            CMAKE_CXX_FLAGS_RELEASE
            "${CMAKE_CXX_FLAGS_RELEASE} -g"
    )
ENDIF()

add_subdirectory(analyzers)
add_subdirectory(board)
add_subdirectory(evaluatealphabeta)
add_subdirectory(evaluatedepthone)
add_subdirectory(evaluatederivative)
add_subdirectory(hashmap)
add_subdirectory(utils)
add_subdirectory(jni)

add_library(
        constants
        constants.h
)

set_target_properties(
        constants PROPERTIES LINKER_LANGUAGE CXX
)
