# Copyright 2024 Michele Borassi
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
    board_optimized
    board_optimized.h
    board_optimized.cpp
)

target_link_libraries(
    board_optimized
    LINK_PUBLIC
    board
    bitpattern
)

IF(${__BMI2__})
    target_compile_options(
        board_optimized
        PUBLIC
        -mbmi2
    )
ENDIF()

IF (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(
            board_optimized
            PUBLIC
            -fconstexpr-steps=100000000
    )
ELSEIF (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
            board_optimized
            PUBLIC
            -fconstexpr-ops-limit=1000000000
    )
ENDIF()

IF(ENABLE_GOOGLETEST)
add_executable(
    board_optimized_test
    board_optimized_test.cpp
)

target_link_libraries(
    board_optimized_test
    LINK_PUBLIC
    board_optimized
    GTest::gtest
    GTest::gtest_main
    gmock
    -no-pie
)
ENDIF()

add_executable(
        board_optimized_flip_profile
        board_optimized_flip_profile.cpp
)

target_link_libraries(
        board_optimized_flip_profile
        LINK_PRIVATE
        board_optimized
        load_training_set
)

add_library(
        compressed_flip
        compressed_flip.h
        compressed_flip.cpp
)

target_link_libraries(
        compressed_flip
        LINK_PUBLIC
        board_optimized
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            compressed_flip_test
            compressed_flip_test.cpp
    )

    target_link_libraries(
            compressed_flip_test
            LINK_PUBLIC
            board
            board_optimized
            compressed_flip
            GTest::gtest
            GTest::gtest_main
            gmock
            -no-pie
    )
ENDIF()

add_library(
        flip_optimized
        flip_optimized.h
        flip_optimized.cpp
)

target_link_libraries(
        flip_optimized
        LINK_PUBLIC
        board_optimized
        compressed_flip
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            flip_optimized_test
            flip_optimized_test.cpp
    )

    target_link_libraries(
            flip_optimized_test
            LINK_PUBLIC
            board
            flip_optimized
            GTest::gtest
            GTest::gtest_main
            gmock
            -no-pie
    )
ENDIF()

add_library(
        play_move
        play_move.h
        play_move.cpp
)

target_link_libraries(
        play_move
        LINK_PUBLIC
        flip_optimized
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            play_move_test
            play_move_test.cpp
    )

    target_link_libraries(
            play_move_test
            LINK_PUBLIC
            board
            play_move
            GTest::gtest
            GTest::gtest_main
            gmock
            -no-pie
    )
ENDIF()