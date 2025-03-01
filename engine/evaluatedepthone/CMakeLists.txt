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
        pattern
        pattern.cpp
        pattern.h
)

target_link_libraries(
        pattern
        LINK_PRIVATE
        bitpattern
        board
)

IF(ENABLE_GOOGLETEST)
add_executable(
        pattern_test
        pattern_test.cpp
)

target_link_libraries(
        pattern_test
        LINK_PRIVATE
        bitpattern
        board
        pattern
        GTest::gtest
        GTest::gtest_main
        gmock
        -no-pie
)
ENDIF()

add_library(
        pattern_evaluator
        pattern_evaluator.h
        pattern_evaluator.cpp
)

target_link_libraries(
        pattern_evaluator
        LINK_PUBLIC
        files
        bitpattern
        board
        evaluator_depth_one_base
)

IF(ENABLE_GOOGLETEST)
add_executable(
        pattern_evaluator_test
        pattern_evaluator_test.cpp
)

target_link_libraries(
        pattern_evaluator_test
        LINK_PUBLIC
        board
        get_moves
        pattern
        pattern_evaluator
        GTest::gtest
        GTest::gtest_main
        gmock
        -no-pie
)
ENDIF()

add_library(
        evaluator_depth_one_base
        evaluator_depth_one_base.h
        evaluator_depth_one_base.cpp
)

add_library(
        test_evaluator_depth_one
        test_evaluator_depth_one.h
        test_evaluator_depth_one.cpp
)

target_link_libraries(
        test_evaluator_depth_one
        LINK_PRIVATE
        board
        evaluator_depth_one_base
)

IF(ENABLE_GOOGLETEST)
add_executable(
        test_evaluator_depth_one_test
        test_evaluator_depth_one_test.cpp
)

target_link_libraries(
        test_evaluator_depth_one_test
        LINK_PRIVATE
        board
        get_moves
        test_evaluator
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        train_pattern_evaluator
        train_pattern_evaluator.h
        train_pattern_evaluator.cpp
)

target_link_libraries(
        train_pattern_evaluator
        LINK_PUBLIC
        misc
        pattern_evaluator
        pattern
)

IF(ENABLE_GOOGLETEST)
add_executable(
        train_pattern_evaluator_test
        train_pattern_evaluator_test.cpp
)

target_link_libraries(
        train_pattern_evaluator_test
        LINK_PRIVATE
        train_pattern_evaluator
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_executable(
        train_pattern_evaluator_main
        train_pattern_evaluator_main.cpp
)

target_link_libraries(
        train_pattern_evaluator_main
        LINK_PRIVATE
        pattern_evaluator
        load_training_set
        train_pattern_evaluator
)
