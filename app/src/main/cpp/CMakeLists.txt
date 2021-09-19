# Copyright 2021 Google LLC
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

# Sets the minimum version of CMake required to build the native library.
cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("othellosensei")

SET(ANDROID FALSE)
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")

enable_testing()

if(ANDROID)
    find_library(log-lib JNI_LIBRARIES)
else()
    find_package(GTest)
    find_package(JNI REQUIRED)
    include_directories( ${JNI_INCLUDE_DIRS} )
endif()

SET(PDEP_PEXT NOT ANDROID)

if(${PDEP_PEXT})
    add_definitions(-DPDEP_PEXT=true)
else()
    add_definitions(-DPDEP_PEXT=false)
endif()

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

if(!ANDROID)
  SET(
          RELEASE_FLAGS
          ${RELEASE_FLAGS}
          -m64
          -march=native
  )
endif()

add_library(
        bitpattern
        bitpattern.h
        bitpattern.cpp
)

target_compile_options(
        bitpattern
        PUBLIC
        ${RELEASE_FLAGS}
)

if(!ANDROID)
    add_executable(
            bitpattern_test
            bitpattern_test.cpp
    )

    target_link_libraries(
            bitpattern_test
            LINK_PUBLIC
            bitpattern
            GTest::gtest
            GTest::gtest_main
    )
endif()

add_library(
        board
        board.h
        board.cpp
)

target_link_libraries(board LINK_PUBLIC bitpattern)

add_library(
        get_flip
        get_flip.h
        get_flip.cpp
)

target_link_libraries(get_flip LINK_PUBLIC bitpattern)

target_compile_options(
        get_flip
        PUBLIC
        ${RELEASE_FLAGS}
)

if(${PDEP_PEXT})
    target_compile_options(
            get_flip
            PUBLIC
            -mbmi2
    )
endif()

if(!ANDROID)
    add_executable(
            get_flip_test
            get_flip_test.cpp
    )

    target_link_libraries(
            get_flip_test
            LINK_PRIVATE
            board
            get_flip
            GTest::gtest
            GTest::gtest_main
    )
endif()

add_executable(
        get_flip_profile
        get_flip_profile.cpp
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
        stable.h
        stable.cpp
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

if(${PDEP_PEXT})
    target_compile_options(
            stable
            PUBLIC
            -mbmi2
            ${RELEASE_FLAGS}
    )
endif()

if(!ANDROID)
    add_executable(
            stable_test
            stable_test.cpp
    )

    target_link_libraries(
            stable_test
            LINK_PRIVATE
            board
            stable
            GTest::gtest
            GTest::gtest_main
    )
endif()

add_library(
        evaluator_last_moves
        SHARED
        evaluator_last_moves.h
        evaluator_last_moves.cpp
)

if(ANDROID)
    target_compile_options(
            evaluator_last_moves
            PUBLIC
            ${RELEASE_FLAGS}
    )
else()
    target_compile_options(
            evaluator_last_moves
            PUBLIC
            ${RELEASE_FLAGS}
    )

endif()

target_link_libraries( # Specifies the target library.
        evaluator_last_moves
        LINK_PUBLIC
        bitpattern
        get_flip
        stable
        ${JNI_LIBRARIES}
)

if(!ANDROID)
    add_executable(
            evaluator_last_moves_test
            evaluator_last_moves_test.cpp
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
endif()

add_executable(
        evaluator_last_moves_profile
        evaluator_last_moves_profile.cpp
)

target_link_libraries(
        evaluator_last_moves_profile
        LINK_PRIVATE
        evaluator_last_moves
)