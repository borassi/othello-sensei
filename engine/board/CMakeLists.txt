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

add_library(
        bitpattern
        bitpattern.h
        bitpattern.cpp
)

target_link_libraries(
        bitpattern
        LINK_PUBLIC
        types
)

IF(ENABLE_GOOGLETEST)
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
        gmock
        -no-pie
)
ENDIF()

add_library(
        board
        board.h
        board.cpp
)

target_link_libraries(
        board
        LINK_PUBLIC
        bitpattern
        get_flip
        get_moves
        misc
)

IF(ENABLE_GOOGLETEST)
add_executable(
        board_test
        board_test.cpp
)

target_link_libraries(
        board_test
        LINK_PUBLIC
        board
        get_moves
        GTest::gtest
        GTest::gtest_main
        gmock
        -no-pie
)
ENDIF()

add_library(
        get_flip
        get_flip.h
        get_flip.cpp
)

target_link_libraries(get_flip LINK_PUBLIC bitpattern)

IF(${__BMI2__})
    target_compile_options(
            get_flip
            PUBLIC
            -mbmi2
    )
ENDIF()

IF(ENABLE_GOOGLETEST)
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
        -no-pie
)
ENDIF()

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

add_library(
        get_moves
        get_moves.h
        get_moves.cpp
)

IF(${__BMI2__})
    target_compile_options(
            get_moves
            PUBLIC
            -mbmi2
    )
ENDIF()

target_link_libraries(
        get_moves
        LINK_PRIVATE
        bitpattern
        board
        get_flip
)

IF(ENABLE_GOOGLETEST)
add_executable(
        get_moves_test
        get_moves_test.cpp
)

target_link_libraries(
        get_moves_test
        LINK_PRIVATE
        bitpattern
        board
        get_flip
        get_moves
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

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

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(
            stable
            PUBLIC
            -fconstexpr-steps=100000000
    )
ELSEIF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
            stable
            PUBLIC
            -fconstexpr-ops-limit=1000000000
    )
ENDIF()

IF(${__BMI2__})
    target_compile_options(
            stable
            PUBLIC
            -mbmi2
    )
ENDIF()

IF(ENABLE_GOOGLETEST)
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
        -no-pie
)
ENDIF()

add_library(
        sequence
        sequence.h
        sequence.cpp
)

target_link_libraries(
        sequence
        LINK_PRIVATE
        bitpattern
        board
        get_flip
        get_moves
        misc
)

IF(ENABLE_GOOGLETEST)
add_executable(
        sequence_test
        sequence_test.cpp
)

target_link_libraries(
        sequence_test
        LINK_PRIVATE
        sequence
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()
