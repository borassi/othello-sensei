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
        hash_map
        hash_map.h
        hash_map.cpp
)

target_link_libraries(
        hash_map
        LINK_PRIVATE
        # atomic
        bitpattern
)

IF(${__BMI2__})
    target_compile_options(
            hash_map
            PUBLIC
            -mbmi2
    )
ENDIF()

IF(ENABLE_GOOGLETEST)
add_executable(
        hash_map_test
        hash_map_test.cpp
)

target_link_libraries(
        hash_map_test
        LINK_PRIVATE
        hash_map
        GTest::gmock
        GTest::gtest
        GTest::gtest_main
        -no-pie
)
ENDIF()