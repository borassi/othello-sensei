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
        thor_test_utils
        thor_test_utils.h
)

set_target_properties(
        thor_test_utils PROPERTIES LINKER_LANGUAGE CXX
)

add_library(
        game
        game.h
        game.cpp
)

add_executable(
        game_test
        game_test.cpp
)

target_link_libraries(
        game_test
        LINK_PRIVATE
        game
        thor_test_utils
        GTest::gtest
        GTest::gtest_main
        -no-pie
)

add_library(
        thor
        thor.h
        thor.cpp
)

target_link_libraries(
        thor
        LINK_PUBLIC
        board
        files
        game
        get_flip
        get_moves
        misc
)

add_executable(
        thor_test
        thor_test.cpp
)

target_link_libraries(
        thor_test
        LINK_PRIVATE
        thor
        thor_test_utils
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)