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
# See the License for the specIFic language governing permissions and
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

add_library(
        random
        utils/random.h
)
set_target_properties(
        random PROPERTIES LINKER_LANGUAGE CXX
)

target_compile_options(
        random
        PUBLIC
        ${RELEASE_FLAGS}
)

IF(NOT ANDROID)
    add_executable(
            random_test
            utils/random_test.cpp
    )

    target_link_libraries(
            random_test
            LINK_PUBLIC
            random
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_library(
        bitpattern
        board/bitpattern.h
        board/bitpattern.cpp
)

target_compile_options(
        bitpattern
        PUBLIC
        ${RELEASE_FLAGS}
)

IF(NOT ANDROID)
    add_executable(
            bitpattern_test
            board/bitpattern_test.cpp
    )

    target_link_libraries(
            bitpattern_test
            LINK_PUBLIC
            bitpattern
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_library(
        board
        board/board.h
        board/board.cpp
)

target_link_libraries(board LINK_PUBLIC bitpattern)

add_library(
        get_moves
        board/get_moves.h
        board/get_moves.cpp
)

target_link_libraries(
        get_moves
        LINK_PRIVATE
        bitpattern
)

target_compile_options(
        get_moves
        PUBLIC
        ${RELEASE_FLAGS}
)

IF(NOT ANDROID)
    add_executable(
            get_moves_test
            board/get_moves_test.cpp
    )

    target_link_libraries(
            get_moves_test
            LINK_PRIVATE
            bitpattern
            board
            get_flip
            get_moves
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_library(
        get_flip
        board/get_flip.h
        board/get_flip.cpp
)

target_link_libraries(get_flip LINK_PUBLIC bitpattern)

target_compile_options(
        get_flip
        PUBLIC
        ${RELEASE_FLAGS}
)

IF(${PDEP_PEXT})
    target_compile_options(
            get_flip
            PUBLIC
            -mbmi2
    )
ENDIF()

IF(NOT ANDROID)
    add_executable(
            get_flip_test
            board/get_flip_test.cpp
    )

    target_link_libraries(
            get_flip_test
            LINK_PRIVATE
            board
            get_flip
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_executable(
        get_flip_profile
        board/get_flip_profile.cpp
)

target_link_libraries(
        get_flip_profile
        LINK_PRIVATE
        bitpattern
        get_flip
)

target_compile_options(
        get_flip_profile
        PUBLIC
        ${RELEASE_FLAGS}
)

add_library(
        stable
        board/stable.h
        board/stable.cpp
)

target_link_libraries(
        stable
        LINK_PRIVATE
        bitpattern
        get_flip
)

target_compile_options(
        stable
        PUBLIC
        -fconstexpr-steps=1000000000
        ${RELEASE_FLAGS}
)

IF(${PDEP_PEXT})
    target_compile_options(
            stable
            PUBLIC
            -mbmi2
            ${RELEASE_FLAGS}
    )
ENDIF()

IF(NOT ANDROID)
    add_executable(
            stable_test
            board/stable_test.cpp
    )

    target_link_libraries(
            stable_test
            LINK_PRIVATE
            board
            stable
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_library(
        hash_map
        hashmap/hash_map.h
        hashmap/hash_map.cpp
)

target_link_libraries(
        hash_map
        LINK_PRIVATE
        bitpattern
)

target_compile_options(
        hash_map
        PUBLIC
        ${RELEASE_FLAGS}
)

IF(${PDEP_PEXT})
    target_compile_options(
            hash_map
            PUBLIC
            -mbmi2
            ${RELEASE_FLAGS}
    )
ENDIF()

IF(NOT ANDROID)
    add_executable(
            hash_map_test
            hashmap/hash_map_test.cpp
    )

    target_link_libraries(
            hash_map_test
            LINK_PRIVATE
            hash_map
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_library(
        evaluator_last_moves
        SHARED
        evaluateindepth/evaluator_last_moves.h
        evaluateindepth/evaluator_last_moves.cpp
)

IF(ANDROID)
    target_compile_options(
            evaluator_last_moves
            PUBLIC
            ${RELEASE_FLAGS}
    )
ELSE()
    target_compile_options(
            evaluator_last_moves
            PUBLIC
            ${RELEASE_FLAGS}
    )

ENDIF()

target_link_libraries( # Specifies the target library.
        evaluator_last_moves
        LINK_PUBLIC
        bitpattern
        get_flip
        hash_map
        stable
        ${JNI_LIBRARIES}
)

IF(NOT ANDROID)
    add_executable(
            evaluator_last_moves_test
            evaluateindepth/evaluator_last_moves_test.cpp
    )

    target_link_libraries(
            evaluator_last_moves_test
            LINK_PRIVATE
            bitpattern
            board
            evaluator_last_moves
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_executable(
        evaluator_last_moves_profile
        evaluateindepth/evaluator_last_moves_profile.cpp
)

target_link_libraries(
        evaluator_last_moves_profile
        LINK_PRIVATE
        evaluator_last_moves
)

add_library(
        test_evaluator
        evaluatedepthone/test_evaluator.h
)

target_link_libraries(
        test_evaluator
        LINK_PRIVATE
        board
)

IF(NOT ANDROID)
    add_executable(
            test_evaluator_test
            evaluatedepthone/test_evaluator_test.cpp
    )

    target_link_libraries(
            test_evaluator_test
            LINK_PRIVATE
            board
            get_moves
            test_evaluator
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()