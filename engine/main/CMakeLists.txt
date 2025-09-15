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
        bindings
        bindings.h
        bindings.cpp
)

target_link_libraries( # Specifies the target library.
        bindings
        LINK_PUBLIC
        types
)

add_library(
        engine
        engine.h
        engine.cpp
)

target_link_libraries( # Specifies the target library.
        engine
        LINK_PUBLIC
        bindings
        board
        book
        evaluator_derivative
        get_flip
        get_moves
        misc
        pattern_evaluator
        state
        thor
)

add_library(
        state
        state.h
        state.cpp
)

target_link_libraries( # Specifies the target library.
        state
        LINK_PUBLIC
        bindings
        board
        move_selector
        sequence
        thor
        tree_node
)

add_library(
        main
        main.h
        main.cpp
)

target_link_libraries( # Specifies the target library.
        main
        LINK_PUBLIC
        bindings
        board
        book
        engine
        evaluator_derivative
        game_to_save
        get_flip
        get_moves
        misc
        pattern_evaluator
        state
        thor
        tree_node
        xot
)

add_library(
        ui
        SHARED
        ui.h
        ui.cpp
)

target_link_libraries( # Specifies the target library.
        ui
        LINK_PUBLIC
        bindings
        main
)

if(CMAKE_SYSTEM_NAME STREQUAL "Android")
    target_link_options(ui PRIVATE "-Wl,-z,max-page-size=16384")
endif()
