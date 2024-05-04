# Copyright 2023 Michele Borassi
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
# cd .../AndroidStudioProjects/OthelloSensei
# cmake -S app/src/main/cpp -B build -DANDROID=FALSE && cmake --build build && ./build/FILE

# Profile:
# cmake -S app/src/main/cpp -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo && \
# cmake --build build --parallel=12 && \
# valgrind --tool=callgrind --callgrind-out-file=/tmp/callgrind.out ./build/analyzers/endgame_ffo_main && \
# kcachegrind /tmp/callgrind.out

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 2.8.12)

# Declares and names the project.

project("othellosensei" CXX)

IF(NOT DEFINED ANDROID)
    SET(ANDROID TRUE)
ENDIF()
IF(NOT DEFINED PROFILE)
    SET(PROFILE FALSE)
ENDIF()

IF(${CMAKE_VERSION} VERSION_GREATER "3.13.0")
    SET(ENABLE_GOOGLETEST TRUE)
ELSE()
    SET(ENABLE_GOOGLETEST FALSE)
    MESSAGE("WARNING: disabled googletest because CMake version ${CMAKE_VERSION} is too old")
ENDIF()

MESSAGE("Compiler: ${CMAKE_CXX_COMPILER_ID}")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS} -pthread -fPIC")

IF(ENABLE_GOOGLETEST)
    INCLUDE(FetchContent)
    FetchContent_Declare(
        googletest
        DOWNLOAD_EXTRACT_TIMESTAMP
        URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
ENDIF()

IF(ANDROID)
    find_library(log-lib JNI_LIBRARIES)
ELSE()
    find_package(JNI REQUIRED)
ENDIF()

#find_library(STDCPPFS_LIBRARY NAMES stdc++fs)

include_directories( ${JNI_INCLUDE_DIRS} )

SET(
        CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -pedantic -pipe -D_GNU_SOURCE=1 -m64 -Ofast -DNDEBUG -march=native -funroll-loops -DUSE_GAS_X64 -DPOPCOUNT -DHAS_CPU_64 -fomit-frame-pointer -DUSE_GCC_ARM"
)

IF (PROFILE)
    SET(
        CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -g"
    )
ENDIF()

add_subdirectory(analyzers)
add_subdirectory(board)
add_subdirectory(book)
add_subdirectory(estimators)
add_subdirectory(evaluatealphabeta)
add_subdirectory(evaluatedepthone)
add_subdirectory(evaluatederivative)
add_subdirectory(hashmap)
add_subdirectory(main)
add_subdirectory(thor)
add_subdirectory(utils)
add_subdirectory(jni)

add_library(
        constants
        constants.h
)

set_target_properties(
        constants PROPERTIES LINKER_LANGUAGE CXX
)