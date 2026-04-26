# Copyright 2023-2026 Michele Borassi
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
# cmake -S engine -B build -DANDROID=FALSE && cmake --build build && ./build/FILE

# Profile:
# cmake -S app/src/main/cpp -B build -DANDROID=FALSE -DCMAKE_BUILD_TYPE=RelWithDebInfo && \
# cmake --build build --parallel=12 && \
# valgrind --tool=callgrind --callgrind-out-file=/tmp/callgrind.out ./build/analyzers/endgame_ffo_main && \
# kcachegrind /tmp/callgrind.out

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.15)

# Declares and names the project.

project("othellosensei" CXX)

# Silences some warnings with the absl library.
add_compile_options(-Wno-gnu-zero-variadic-macro-arguments)
add_compile_options(-Wno-gcc-compat)

if(EMSCRIPTEN)
    # Suppress warnings about using deprecated Emscripten macros in 3rd party libs
    add_compile_options(
            "-Wno-deprecated"
            "-Wno-deprecated-pragma"
            "-Wno-deprecated-declarations"
    )
    # Apply optimizations to ALL targets in the project
    add_compile_options(
            "-O3"
            "-flto"
            "-pthread"
            "-msimd128"
    )
endif()

include(FetchContent)

IF(NOT DEFINED ANDROID)
    SET(ANDROID TRUE)
ENDIF()
IF(NOT DEFINED PROFILE)
    SET(PROFILE FALSE)
ENDIF()

# Change to disable tests.
SET(ENABLE_GOOGLETEST TRUE)

MESSAGE("Compiler: ${CMAKE_CXX_COMPILER_ID}")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_FLAGS "-std=c++17 ${CMAKE_CXX_FLAGS} -pthread -fPIC")

IF(ENABLE_GOOGLETEST)
    FetchContent_Declare(
      googletest
      DOWNLOAD_EXTRACT_TIMESTAMP ON
      URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
ENDIF()

FetchContent_Declare(
        absl
        GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
        GIT_TAG        20260107.1 # Use a recent LTS release tag
)
FetchContent_MakeAvailable(absl)

SET(
        CMAKE_CXX_FLAGS_RELEASE
        "${CMAKE_CXX_FLAGS_RELEASE} -pedantic -pipe -O3 -ffast-math -DNDEBUG -funroll-loops -fomit-frame-pointer"
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
add_subdirectory(book_visitor)
add_subdirectory(estimators)
add_subdirectory(evaluatealphabeta)
add_subdirectory(evaluatedepthone)
add_subdirectory(evaluatederivative)
add_subdirectory(hashmap)
add_subdirectory(main)
add_subdirectory(playagainstsensei)
add_subdirectory(savegame)
add_subdirectory(thor)
add_subdirectory(utils)
add_subdirectory(xot)

if (EMSCRIPTEN)
    add_subdirectory(wasm)
ENDIF()