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

add_executable(
        collect_examples
        collect_examples.cpp
)

target_link_libraries(
        collect_examples
        LINK_PRIVATE
        board
        evaluator_derivative
        hash_map
        load_training_set
        parse_flags
        pattern_evaluator
)

add_library(
        endgame_ffo
        endgame_ffo.h
        endgame_ffo.cpp
)

target_link_libraries(
        endgame_ffo
        LINK_PUBLIC
        board
)

add_executable(
        test_memory_main
        test_memory_main.cpp
)

target_link_libraries(
        test_memory_main
        LINK_PUBLIC
        book
        evaluator_alpha_beta
        evaluator_derivative
        hash_map
        misc
        parse_flags
        pattern_evaluator
        thor
)

add_executable(
        evaluate_cli_main
        evaluate_cli_main.cpp
)

target_link_libraries(
        evaluate_cli_main
        LINK_PUBLIC
        board
        evaluator_alpha_beta
        evaluator_derivative
        hash_map
        load_training_set
        misc
        parse_flags
        pattern_evaluator
        sequence
)

add_executable(
        endgame_ffo_main
        endgame_ffo_main.cpp
)

target_link_libraries(
        endgame_ffo_main
        LINK_PUBLIC
        evaluator_alpha_beta
        evaluator_derivative
        hash_map
        load_training_set
        misc
        parse_flags
        pattern_evaluator
)

add_executable(
        thor_main
        thor_main.cpp
)

target_link_libraries(
        thor_main
        LINK_PUBLIC
        bitpattern
        evaluator_alpha_beta
        get_moves
        load_training_set
        misc
        pattern_evaluator
        test_evaluator
)

add_executable(
        thor_endgame
        thor_endgame.cpp
)

target_link_libraries(
        thor_endgame
        LINK_PUBLIC
        bitpattern
        evaluator_alpha_beta
        get_moves
        load_training_set
        misc
        pattern_evaluator
        test_evaluator
)

add_executable(
        last_moves
        last_moves.cpp
)

target_link_libraries(
        last_moves
        LINK_PRIVATE
        evaluator_alpha_beta
        pattern_evaluator
)

add_executable(
        position_to_thor_scores_main
        position_to_thor_scores_main.cpp
)

target_link_libraries(
        position_to_thor_scores_main
        LINK_PRIVATE
        board
        book
        misc
        parse_flags
        thor
)