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
        random
        random.h
)
set_target_properties(
        random PROPERTIES LINKER_LANGUAGE CXX
)

target_compile_options(
        random
        PUBLIC
        ${RELEASE_FLAGS}
)

IF(NOT ANDROID)
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
    )
ENDIF()

add_library(
        misc
        misc.h
)

set_target_properties(
        misc PROPERTIES LINKER_LANGUAGE CXX
)

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
