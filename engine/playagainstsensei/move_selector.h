/*
 * Copyright 2025 Michele Borassi
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OTHELLO_SENSEI_PLAYAGAINSTSENSEI_MOVE_SELECTOR_H
#define OTHELLO_SENSEI_PLAYAGAINSTSENSEI_MOVE_SELECTOR_H

#include "../evaluatederivative/tree_node.h"

// Returns how much of the total game error should happen when playing this move.
double GetMoveMultiplier(int move);

// Returns the next move, with an expected error equal to the value provided.
//
// If there are n moves move_0, ..., move_n with evaluations eval_0 > eval_1 > ... > eval_n,
// let error_i = eval_i - eval_0, and let p_i the probability that we choose move_i.
// We define p_i = exp(-lambda error_i), and we use binary search to find the lambda that achieves
// the desired expected_error.
//
// For lambda = 0, we choose a random move; for lambda = infinity, we choose the best move.
//
// Advantages:
// - The probability of making a mistake of X disks decreases exponentially with X, which is
//   something we see in real games.
// - The probability of a better move is always higher than the probability of a worse move.
const Node* FindNextMove(const std::vector<const Node*>& children, double expected_error, double max_error);

// Same as FindNextMove, but rescales the total_error so that that's the error at the end of the game.
const Node* FindNextMoveTotalError(const std::vector<const Node*>& children, double total_error, double max_error);

#endif  // OTHELLO_SENSEI_PLAYAGAINSTSENSEI_MOVE_SELECTOR_H