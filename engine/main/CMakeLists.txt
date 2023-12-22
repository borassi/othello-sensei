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
        main
        SHARED
        bindings.h
        bindings.cpp
        main.h
        main.cpp
)

target_link_libraries( # Specifies the target library.
        main
        LINK_PUBLIC
        board
        book
        evaluator_derivative
        get_flip
        get_moves
        misc
        pattern_evaluator
)