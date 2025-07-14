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
        thor_test_utils.cpp
)

add_library(
        game
        game.h
        game.cpp
)

target_link_libraries(
        game
        LINK_PRIVATE
        board
        sequence
)

IF(ENABLE_GOOGLETEST)
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
ENDIF()

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
        saved_game_list
        source
)

IF(ENABLE_GOOGLETEST)
add_executable(
        thor_test
        thor_test.cpp
)

target_link_libraries(
        thor_test
        LINK_PRIVATE
        files
        thor
        thor_test_utils
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        generic_source
        generic_source.h
        generic_source.cpp
)

target_link_libraries(
        generic_source
        LINK_PUBLIC
        game
        sequence
)

add_library(
        source
        source.h
        source.cpp
)

target_link_libraries(
        source
        LINK_PUBLIC
        files
        game
        generic_source
        sequence
)

IF(ENABLE_GOOGLETEST)
add_executable(
        source_test
        source_test.cpp
)

target_link_libraries(
        source_test
        LINK_PRIVATE
        files
        sequence
        source
        thor_test_utils
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_executable(
        thor_analyze_errors_main
        thor_analyze_errors_main.cpp
)

target_link_libraries(
        thor_analyze_errors_main
        LINK_PUBLIC
        book
        evaluator_derivative
        hash_map
        parse_flags
        pattern_evaluator
        thor
        xot
)

add_executable(
        thor_create_extra_files
        thor_create_extra_files.cpp
)

target_link_libraries(
        thor_create_extra_files
        LINK_PUBLIC
        fix_xot_source
        parse_flags
        thor
        xot
)

add_executable(
        thor_profile
        thor_profile.cpp
)

target_link_libraries(
        thor_profile
        LINK_PUBLIC
        parse_flags
        thor
)

add_library(
        fix_xot_source
        fix_xot_source.h
        fix_xot_source.cpp
)

target_link_libraries(
        fix_xot_source
        LINK_PUBLIC
        source
        xot
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            fix_xot_source_test
            fix_xot_source_test.cpp
    )

    target_link_libraries(
            fix_xot_source_test
            LINK_PRIVATE
            fix_xot_source
            thor_test_utils
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()

add_executable(
        thor_to_saved_games_main
        thor_to_saved_games_main.cpp
)

target_link_libraries(
        thor_to_saved_games_main
        LINK_PUBLIC
        game_to_save
        parse_flags
        thor
)