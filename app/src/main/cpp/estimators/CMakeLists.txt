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
        ../estimators/endgame_time.h
)

target_link_libraries( # Specifies the target library.
        endgame_time
        LINK_PUBLIC
        bitpattern
        get_moves
)

set_target_properties(
        endgame_time PROPERTIES LINKER_LANGUAGE CXX
)

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

add_library(
        gaussian
        gaussian.h
        gaussian.cpp
)

add_executable(
        gaussian_test
        gaussian_test.cpp
)

target_link_libraries(
        gaussian_test
        LINK_PUBLIC
        gaussian
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
