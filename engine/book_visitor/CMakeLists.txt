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
        book_visitor_test_utils
        book_visitor_test_utils.h
        book_visitor_test_utils.cpp
)

target_link_libraries(
        book_visitor_test_utils
        LINK_PRIVATE
        visitor
)

add_library(
        visitor
        visitor.h
        visitor.cpp
)

target_link_libraries(
        visitor
        LINK_PRIVATE
        book
        get_moves
        sequence
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            visitor_test
            visitor_test.cpp
    )

    target_link_libraries(
            visitor_test
            LINK_PRIVATE
            book_visitor_test_utils
            visitor
            test_utils
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_library(
        merger
        merger.h
        merger.cpp
)

target_link_libraries(
        merger
        LINK_PRIVATE
        book
        visitor
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            merger_test
            merger_test.cpp
    )

    target_link_libraries(
            merger_test
            LINK_PRIVATE
            book_visitor_test_utils
            visitor
            test_utils
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_executable(
        reduce_book_main
        reduce_book_main.cpp
)

target_link_libraries(
        reduce_book_main
        LINK_PRIVATE
        book
        visitor
        constants
        misc
        parse_flags
        thor
        win_probability
)

add_executable(
        book_visitor_stats_main
        book_visitor_stats_main.cpp
)

target_link_libraries(
        book_visitor_stats_main
        LINK_PRIVATE
        book
        visitor
        constants
        misc
        parse_flags
        thor
        win_probability
)

add_executable(
        expand_book_auto_main
        expand_book_auto_main.cpp
)

target_link_libraries(
        expand_book_auto_main
        LINK_PRIVATE
        book
        evaluator_alpha_beta
        evaluator_derivative
        files
        hash_map
        misc
        parse_flags
        pattern_evaluator
        thor
        tree_node
        visitor
)

add_executable(
        merge_books_main
        merge_books_main.cpp
)

target_link_libraries(
        merge_books_main
        LINK_PRIVATE
        book
        merger
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