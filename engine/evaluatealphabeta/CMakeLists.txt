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
        evaluator_last_moves
        evaluator_last_moves.h
        evaluator_last_moves.cpp
)

target_link_libraries( # Specifies the target library.
        evaluator_last_moves
        LINK_PUBLIC
        bitpattern
        get_flip
        hash_map
        misc
        stable
)

IF(ENABLE_GOOGLETEST)
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
        -no-pie
)
ENDIF()

add_library(
        evaluator_alpha_beta
        evaluator_alpha_beta.h
        evaluator_alpha_beta.cpp
)

target_link_libraries( # Specifies the target library.
        evaluator_alpha_beta
        LINK_PUBLIC
        bitpattern
        endgame_time
        evaluator_last_moves
        win_probability
        get_flip
        hash_map
        stable)

IF(ENABLE_GOOGLETEST)
add_executable(
        evaluator_alpha_beta_test
        evaluator_alpha_beta_test.cpp
)

target_link_libraries(
        evaluator_alpha_beta_test
        LINK_PRIVATE
        bitpattern
        board
        evaluator_alpha_beta
        pattern_evaluator
        test_evaluator
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        test_evaluator
        test_evaluator.h
        test_evaluator.cpp
)

target_link_libraries(
        test_evaluator
        LINK_PRIVATE
        board
        get_moves
)

IF(ENABLE_GOOGLETEST)
add_executable(
        test_evaluator_test
        test_evaluator_test.cpp
)

target_link_libraries(
        test_evaluator_test
        LINK_PRIVATE
        board
        test_evaluator
        test_evaluator_depth_one
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()