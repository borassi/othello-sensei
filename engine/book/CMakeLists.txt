# Copyright 2022-2024 Michele Borassi
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
        value_file
        value_file.h
        value_file.cpp
)

target_link_libraries(
        value_file
        LINK_PRIVATE
        files
)

IF(ENABLE_GOOGLETEST)
add_executable(
        value_file_test
        value_file_test.cpp
)

target_link_libraries(
        value_file_test
        LINK_PRIVATE
        files
        value_file
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        book
        book.h
        book.cpp
)

target_link_libraries(
        book
        LINK_PRIVATE
        bitpattern
        board
        book_tree_node
        constants
        files
        value_file
)

IF(ENABLE_GOOGLETEST)
add_executable(
        book_test
        book_test.cpp
)

target_link_libraries(
        book_test
        LINK_PRIVATE
        board
        book
        book_tree_node
        misc
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        test_utils
        test_utils.h
)

set_target_properties(
        test_utils PROPERTIES LINKER_LANGUAGE CXX
)

target_link_libraries(
        test_utils
        LINK_PRIVATE
        book
        book_tree_node
        get_moves
        tree_node
)

add_library(
        book_tree_node
        book_tree_node.h
        book_tree_node.cpp
)

target_link_libraries(
        book_tree_node
        LINK_PRIVATE
        endgame_time
        evaluation
        serializable_boolean_vector
        tree_node
        value_file
)

IF(ENABLE_GOOGLETEST)
add_executable(
        book_tree_node_test
        book_tree_node_test.cpp
)

target_link_libraries(
        book_tree_node_test
        LINK_PRIVATE
        board
        book_tree_node
        get_moves
        test_utils
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_executable(
        merge_books_main
        merge_books_main.cpp
)

target_link_libraries(
        merge_books_main
        LINK_PRIVATE
        book
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