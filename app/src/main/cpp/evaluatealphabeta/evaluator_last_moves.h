/*
 * Copyright 2022 Google LLC
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

#ifndef EVALUATOR_LAST_MOVES_H
#define EVALUATOR_LAST_MOVES_H

#include "../board/get_flip.h"
#include "../hashmap/hash_map.h"

Eval max(Eval e1, Eval e2) {
  return e1 > e2 ? e1 : e2;
}

inline Eval EvalOneEmpty(Square x, BitPattern player, BitPattern opponent) noexcept __attribute__((always_inline));
inline Eval EvalOneEmpty(Square x, BitPattern player, BitPattern opponent) noexcept {
  BitPattern flip = GetFlip(x, player, opponent);
  if (__builtin_expect(flip, 1)) {
    return (__builtin_popcountll(NewOpponent(flip, player)) << 1) - 64;
  }
  flip = GetFlip(x, opponent, player);
  if (flip) {
    return 64 - (__builtin_popcountll(opponent | flip) << 1);
  }
  Eval playerDisks = __builtin_popcountll(player) << 1;
  return playerDisks - (playerDisks >= 64 ? 62 : 64);
}

Eval EvalTwoEmpties(
  const Square x1, const Square x2, const BitPattern player,
  const BitPattern opponent, const Eval lower, const Eval upper,
  int* const n_visited) noexcept;
Eval EvalThreeEmpties(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited);
Eval EvalFourEmpties(
  const Square x1, const Square x2, const Square x3, const Square x4,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper, const bool swap,
  const BitPattern last_flip, const BitPattern stable,
  int* const n_visited);
Eval EvalFiveEmpties(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    const BitPattern last_flip, const BitPattern stable,
    int* const n_visited);

class EvaluatorLastMoves {
 public:
  EvaluatorLastMoves(HashMap* hash_map) : hash_map_(hash_map) {}
  Eval Evaluate(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, int* const n_visited);

 private:
  HashMap* hash_map_;
  Eval Evaluate(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, bool passed,
    const BitPattern last_flip, const BitPattern stable,
    int* const n_visited);
  EpochValue epoch_;
};

#endif /* EVALUATOR_LAST_MOVES_H */

