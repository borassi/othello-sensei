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
        value_file
        value_file.h
        value_file.cpp
)

target_link_libraries(
        value_file
        LINK_PRIVATE
        file
)

add_executable(
        value_file_test
        value_file_test.cpp
)

target_link_libraries(
        value_file_test
        LINK_PRIVATE
        value_file
        GTest::gtest
        GTest::gtest_main
        -no-pie
)

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
        value_file
)

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
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)

add_library(
        book_tree_node
        book_tree_node.h
        book_tree_node.cpp
)

target_link_libraries(
        book_tree_node
        LINK_PRIVATE
        endgame_time_estimator
        evaluation
        tree_node
        value_file
)

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
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)