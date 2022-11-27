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

add_library(
        prob_combiners
        SHARED
        prob_combiners.h
)

set_target_properties(
        prob_combiners PROPERTIES LINKER_LANGUAGE CXX
)

target_link_libraries( # Specifies the target library.
        prob_combiners
        LINK_PUBLIC
        misc
)

target_compile_options(
        prob_combiners
        PUBLIC
        -fconstexpr-steps=100000000
)

IF (NOT ANDROID)
    add_executable(
            prob_combiners_test
            prob_combiners_test.cpp
    )

    target_link_libraries(
            prob_combiners_test
            LINK_PRIVATE
            prob_combiners
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF ()

add_library(
        evaluation
        SHARED
        evaluation.h
        evaluation.cpp
)

target_link_libraries( # Specifies the target library.
        evaluation
        LINK_PUBLIC
        bitpattern
        misc
        prob_combiners
)

IF (NOT ANDROID)
    add_executable(
            evaluation_test
            evaluation_test.cpp
    )

    target_link_libraries(
            evaluation_test
            LINK_PRIVATE
            evaluation
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF ()

add_library(
        endgame_time_estimator
        endgame_time_estimator.h
)

set_target_properties(
        endgame_time_estimator PROPERTIES LINKER_LANGUAGE CXX
)

IF (NOT ANDROID)
    add_executable(
            endgame_time_estimator_test
            endgame_time_estimator_test.cpp
    )

    target_link_libraries(
            endgame_time_estimator_test
            LINK_PRIVATE
            board
            endgame_time_estimator
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF ()

add_library(
        tree_node
        SHARED
        tree_node.h
        tree_node.cpp
)

target_link_libraries( # Specifies the target library.
        tree_node
        LINK_PUBLIC
        bitpattern
        board
        endgame_time_estimator
        evaluation
        get_moves
        misc
)

add_executable(
        tree_node_test
        tree_node_test.cpp
)

target_link_libraries(
        tree_node_test
        LINK_PRIVATE
        board
        tree_node
        GTest::gtest
        GTest::gtest_main
        -no-pie
)

add_library(
        evaluator_derivative
        SHARED
        evaluator_derivative.h
        evaluator_derivative.cpp
)

target_link_libraries( # Specifies the target library.
        evaluator_derivative
        LINK_PUBLIC
        bitpattern
        board
        constants
        get_moves
        misc
        tree_node
        evaluator_depth_one_base
)

add_executable(
        evaluator_derivative_test
        evaluator_derivative_test.cpp
)

target_link_libraries(
        evaluator_derivative_test
        LINK_PRIVATE
        evaluator_alpha_beta
        evaluator_derivative
        hash_map
        pattern_evaluator
        GTest::gtest
        GTest::gtest_main
        -no-pie
)