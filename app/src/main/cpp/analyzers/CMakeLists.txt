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

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

IF (ANDROID)
    find_library(log-lib JNI_LIBRARIES)
ELSE ()
    find_package(GTest)
    find_package(JNI REQUIRED)
    include_directories(${JNI_INCLUDE_DIRS})
ENDIF ()

add_executable(
        evaluate_thor_main
        evaluate_thor_main.cpp
)

target_link_libraries(
        evaluate_thor_main
        LINK_PUBLIC
        bitpattern
        evaluator_alpha_beta
        get_moves
        load_training_set
        test_evaluator
        pattern_evaluator
)

add_executable(
        evaluate_last_moves
        evaluate_last_moves.cpp
)

target_link_libraries(
        evaluate_last_moves
        LINK_PRIVATE
        evaluator_alpha_beta
        pattern_evaluator
)