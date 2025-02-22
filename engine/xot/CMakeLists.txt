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
        xot
        xot.h
        xot.cpp
)

target_link_libraries(
        xot
        LINK_PRIVATE
        board
        misc
        sequence
)

IF(ENABLE_GOOGLETEST)
    add_executable(
            xot_test
            xot_test.cpp
    )

    target_link_libraries(
            xot_test
            LINK_PRIVATE
            board
            sequence
            xot
            thor_test_utils
            GTest::gmock
            GTest::gtest
            GTest::gtest_main
            -no-pie
    )
ENDIF()
