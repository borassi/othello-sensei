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

SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")

IF(ANDROID)
    find_library(log-lib JNI_LIBRARIES)
ELSE()
    find_package(GTest)
    find_package(JNI REQUIRED)
    include_directories( ${JNI_INCLUDE_DIRS} )
ENDIF()


add_library(
        pattern_evaluator
        pattern_evaluator.h
)

target_link_libraries(
        pattern_evaluator
        LINK_PRIVATE
        bitpattern
        board
)

IF(NOT ANDROID)
    add_executable(
            pattern_evaluator_test
            pattern_evaluator_test.cpp
    )

    target_link_libraries(
            pattern_evaluator_test
            LINK_PRIVATE
            board
            test_evaluator
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_library(
        test_evaluator
        test_evaluator.h
)

target_link_libraries(
        test_evaluator
        LINK_PRIVATE
        board
)

IF(NOT ANDROID)
    add_executable(
            test_evaluator_test
            test_evaluator_test.cpp
    )

    target_link_libraries(
            test_evaluator_test
            LINK_PRIVATE
            board
            get_moves
            test_evaluator
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()