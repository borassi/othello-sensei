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
        prob_combiners
        prob_combiners.h
        prob_combiners.cpp
)

target_link_libraries(
        prob_combiners
        LINK_PUBLIC
        functions
        misc
)

IF(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(
            prob_combiners
            PUBLIC
            -fconstexpr-steps=100000000
    )
ELSEIF(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(
            prob_combiners
            PUBLIC
            -fconstexpr-ops-limit=1000000000
    )
ENDIF()

IF(ENABLE_GOOGLETEST)
add_executable(
        prob_combiners_test
        prob_combiners_test.cpp
)

target_link_libraries(
        prob_combiners_test
        LINK_PRIVATE
        prob_combiners
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        evaluation
        evaluation.h
        evaluation.cpp
)

target_link_libraries( # Specifies the target library.
        evaluation
        LINK_PRIVATE
        bitpattern
        endgame_time
        win_probability
        prob_combiners
)

IF(ENABLE_GOOGLETEST)
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
ENDIF()

add_library(
        tree_node
        tree_node.h
        tree_node.cpp
)

target_link_libraries( # Specifies the target library.
        tree_node
        LINK_PUBLIC
        bitpattern
        board
        evaluation
        get_moves
        misc
        serializable_boolean_vector
)

IF(ENABLE_GOOGLETEST)
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
ENDIF()

add_library(
        evaluator_derivative
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
        tree_node
        evaluator_alpha_beta
        evaluator_depth_one_base
)

IF(ENABLE_GOOGLETEST)
add_executable(
        evaluator_derivative_test
        evaluator_derivative_test.cpp
)

target_link_libraries(
        evaluator_derivative_test
        LINK_PRIVATE
        evaluator_alpha_beta
        evaluator_derivative
        win_probability
        hash_map
        pattern_evaluator
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()