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
        move_selector
        move_selector.h
        move_selector.cpp
)

target_link_libraries(
        move_selector
        LINK_PUBLIC
        tree_node
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            move_selector_test
            move_selector_test.cpp
    )

    target_link_libraries(
            move_selector_test
            LINK_PRIVATE
            move_selector
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_executable(
        move_selector_e2e_test
        move_selector_e2e_test.cpp
)

target_link_libraries(
        move_selector_e2e_test
        LINK_PUBLIC
        book
        evaluator_derivative
        hash_map
        move_selector
        parse_flags
        pattern_evaluator
        thor
        xot
)