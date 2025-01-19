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
        book_visitor
        visitor.h
        visitor.cpp
)

target_link_libraries(
        book_visitor
        LINK_PRIVATE
        book
        get_moves
        sequence
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            book_visitor_test
            visitor_test.cpp
    )

    target_link_libraries(
            book_visitor_test
            LINK_PRIVATE
            board
            book
            book_visitor
            test_utils
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_executable(
        book_visitor_stats_main
        book_visitor_stats_main.cpp
)

target_link_libraries(
        book_visitor_stats_main
        LINK_PRIVATE
        book
        book_visitor
        constants
        misc
        parse_flags
        thor
        win_probability
)

add_executable(
        merge_books_main
        merge_books_main.cpp
)

target_link_libraries(
        merge_books_main
        LINK_PRIVATE
        book
        book_visitor
        constants
        parse_flags
        win_probability
)

add_executable(
        expand_book_main
        expand_book_main.cpp
)

target_link_libraries(
        expand_book_main
        LINK_PRIVATE
        book
        book_tree_node
        evaluator_alpha_beta
        evaluator_derivative
        files
        hash_map
        misc
        parse_flags
        pattern_evaluator
)

add_executable(
        expand_book_thor_main
        expand_book_thor_main.cpp
)

target_link_libraries(
        expand_book_thor_main
        LINK_PRIVATE
        files
        board
        book
        book_tree_node
        evaluator_alpha_beta
        evaluator_derivative
        hash_map
        thor
        misc
        parse_flags
        pattern_evaluator
)