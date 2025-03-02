# Copyright 2025 Michele Borassi
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
        evaluator_alpha_beta_optimized
        evaluator_alpha_beta_optimized.h
        evaluator_alpha_beta_optimized.cpp
)

target_link_libraries( # Specifies the target library.
        evaluator_alpha_beta_optimized
        LINK_PUBLIC
        play_move
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            evaluator_alpha_beta_optimized_test
            evaluator_alpha_beta_optimized_test.cpp
    )

    target_link_libraries(
            evaluator_alpha_beta_optimized_test
            LINK_PRIVATE
            bitpattern
            board
            evaluator_alpha_beta
            pattern_evaluator
            test_evaluator
            evaluator_alpha_beta_optimized
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_executable(
        evaluator_alpha_beta_profile
        evaluator_alpha_beta_profile.cpp
)

target_link_libraries(
        evaluator_alpha_beta_profile
        LINK_PRIVATE
        evaluator_alpha_beta_optimized
        load_training_set
)