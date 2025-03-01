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
        endgame_time
        endgame_time.h
        endgame_time.cpp
)

target_link_libraries( # Specifies the target library.
        endgame_time
        LINK_PUBLIC
        bitpattern
        get_moves
        misc
)

IF(ENABLE_GOOGLETEST)
add_executable(
        endgame_time_test
        ../estimators/endgame_time_test.cpp
)

target_link_libraries(
        endgame_time_test
        LINK_PRIVATE
        board
        endgame_time
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        win_probability
        win_probability.h
        win_probability.cpp
)

target_link_libraries(
        win_probability
        LINK_PUBLIC
        functions
)

IF(ENABLE_GOOGLETEST)
add_executable(
        win_probability_test
        win_probability_test.cpp
)

target_link_libraries(
        win_probability_test
        LINK_PUBLIC
        bitpattern
        win_probability
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()