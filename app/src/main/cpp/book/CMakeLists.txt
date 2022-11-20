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

IF(NOT ANDROID)
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
ENDIF()
add_library(
        position_to_data
        position_to_data.h
        position_to_data.cpp
)

target_link_libraries(
        position_to_data
        LINK_PRIVATE
        bitpattern
        board
        value_file
)

IF(NOT ANDROID)
    add_executable(
            position_to_data_test
            position_to_data_test.cpp
    )

    target_link_libraries(
            position_to_data_test
            LINK_PRIVATE
            board
            position_to_data
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()