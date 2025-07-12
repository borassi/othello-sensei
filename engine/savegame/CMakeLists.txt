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
        game_to_save
        game_to_save.h
        game_to_save.cpp
)

target_link_libraries(
        game_to_save
        LINK_PRIVATE
        files
        game
        get_moves
        sequence
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            game_to_save_test
            game_to_save_test.cpp
    )

    target_link_libraries(
            game_to_save_test
            LINK_PRIVATE
            files
            game_to_save
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_library(
        saved_game_list
        saved_game_list.h
        saved_game_list.cpp
)

target_link_libraries(
        saved_game_list
        LINK_PRIVATE
        files
        game
        game_to_save
        sequence
        thor
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            saved_game_list_test
            saved_game_list_test.cpp
    )

    target_link_libraries(
            saved_game_list_test
            LINK_PRIVATE
            saved_game_list
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()