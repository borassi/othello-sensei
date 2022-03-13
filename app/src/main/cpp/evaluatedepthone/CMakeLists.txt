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
        pattern
        pattern.cpp
        pattern.h
)

target_link_libraries(
        pattern
        LINK_PRIVATE
        bitpattern
        board
)

IF(NOT ANDROID)
    add_executable(
            pattern_test
            pattern_test.cpp
    )

    target_link_libraries(
            pattern_test
            LINK_PRIVATE
            bitpattern
            board
            pattern
            GTest::gtest
            GTest::gtest_main
            gmock
    )
ENDIF()


add_library(
        evaluator
        evaluator.h
        evaluator.cpp
)

target_link_libraries(
        evaluator
        LINK_PRIVATE
        bitpattern
        board
)

IF(NOT ANDROID)
    add_executable(
            evaluator_test
            evaluator_test.cpp
    )

    target_link_libraries(
            evaluator_test
            LINK_PUBLIC
            board
            evaluator
            get_moves
            pattern
            GTest::gtest
            GTest::gtest_main
            gmock
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

add_library(
        train_pattern_evaluator
        train_pattern_evaluator.h
        train_pattern_evaluator.cpp
)

target_link_libraries(
        train_pattern_evaluator
        LINK_PRIVATE
        evaluator
        pattern
)

IF(NOT ANDROID)
    add_executable(
            train_pattern_evaluator_test
            train_pattern_evaluator_test.cpp
    )

    target_link_libraries(
            train_pattern_evaluator_test
            LINK_PRIVATE
            train_pattern_evaluator
            GTest::gtest
            GTest::gtest_main
    )
ENDIF()

add_executable(
        train_pattern_evaluator_main
        train_pattern_evaluator_main.cpp
)

target_link_libraries(
        train_pattern_evaluator_main
        LINK_PRIVATE
        evaluator
        load_training_set
        train_pattern_evaluator
)
