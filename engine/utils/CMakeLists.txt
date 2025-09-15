# Copyright 2023-2025 Michele Borassi
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
        files
        files.h
        files.cpp
)

target_link_libraries(
        files
        misc
)

add_library(
        cpu_adapter
        SHARED
        cpu_adapter.h
        cpu_adapter.cpp
)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_options(cpu_adapter PRIVATE "-Wl,-z,max-page-size=16384")
endif()

add_library(
        functions
        functions.h
        functions.cpp
)

IF(ENABLE_GOOGLETEST)
add_executable(
        functions_test
        functions_test.cpp
)

target_link_libraries(
        functions_test
        LINK_PUBLIC
        functions
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        random
        random.h
        random.cpp
)

IF(ENABLE_GOOGLETEST)
add_executable(
        random_test
        random_test.cpp
)

target_link_libraries(
        random_test
        LINK_PUBLIC
        random
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        misc
        misc.h
        misc.cpp
)

target_link_libraries(
        misc
        LINK_PUBLIC
        types
)

IF(ENABLE_GOOGLETEST)
add_executable(
        misc_test
        misc_test.cpp
)

target_link_libraries(
        misc_test
        LINK_PUBLIC
        misc
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        parse_flags
        parse_flags.h
        parse_flags.cpp
)

IF(ENABLE_GOOGLETEST)
add_executable(
        parse_flags_test
        parse_flags_test.cpp
)

target_link_libraries(
        parse_flags_test
        LINK_PUBLIC
        parse_flags
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        load_training_set
        load_training_set.h
        load_training_set.cpp
)

target_link_libraries(
        load_training_set
        LINK_PUBLIC
        bitpattern
        board
)

add_executable(
        load_training_set_main
        load_training_set_main.cpp
)

target_link_libraries(
        load_training_set_main
        LINK_PRIVATE
        load_training_set
)

add_library(
        serializable_boolean_vector
        serializable_boolean_vector.h
        serializable_boolean_vector.cpp
)

IF(ENABLE_GOOGLETEST)
add_executable(
        serializable_boolean_vector_test
        serializable_boolean_vector_test.cpp
)

target_link_libraries(
        serializable_boolean_vector_test
        LINK_PRIVATE
        serializable_boolean_vector
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()

add_library(
        load_thor
        load_thor.h
        load_thor.cpp
)

target_link_libraries(
        load_thor
        LINK_PRIVATE
        files
)

add_library(
        types
        types.h
        types.cpp
)

add_library(
        constants
        constants.h
        constants.cpp
)

target_link_libraries(
        constants
        LINK_PUBLIC
        types
)