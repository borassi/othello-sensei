/*
 * Copyright 2023 Michele Borassi
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
#include "../utils/misc.h"

forceinline(Eval EvalOneEmpty(Square x, BitPattern player, BitPattern opponent) noexcept);
inline Eval EvalOneEmpty(Square x, BitPattern player, BitPattern opponent) noexcept {
  BitPattern flip = GetFlip(x, player, opponent);
  if (__builtin_expect(flip, 1)) {
    return (Eval) ((__builtin_popcountll(NewOpponent(flip, player)) << 1) - 64);
  }
  flip = GetFlip(x, opponent, player);
  if (flip) {
    return (Eval) (64 - (__builtin_popcountll(opponent | flip) << 1));
  }
  Eval playerDisks = (Eval) (__builtin_popcountll(player) << 1);
  return (Eval) (playerDisks - (playerDisks >= 64 ? 62 : 64));
}

forceinline(Eval EvalTwoEmptiesOrMin(
    const Square x1, const Square x2, const BitPattern player,
    const BitPattern opponent, const Eval upper, int* const n_visited) noexcept);

inline Eval EvalTwoEmptiesOrMin(
    const Square x1, const Square x2, const BitPattern player,
    const BitPattern opponent, const Eval upper, int* const n_visited) noexcept {
  (*n_visited)++;
  Eval eval = kLessThenMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    (*n_visited)++;
    eval = -EvalOneEmpty(x2, NewPlayer(flip, opponent), NewOpponent(flip, player));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    (*n_visited)++;
    return MaxEval(eval, -EvalOneEmpty(x1, NewPlayer(flip, opponent), NewOpponent(flip, player)));
  }
  return eval;
}

forceinline(Eval EvalTwoEmpties(
  const Square x1, const Square x2, const BitPattern player,
  const BitPattern opponent, const Eval lower, const Eval upper,
  int* const n_visited) noexcept);

inline Eval EvalTwoEmpties(
  const Square x1, const Square x2, const BitPattern player,
  const BitPattern opponent, const Eval lower, const Eval upper,
  int* const n_visited) noexcept {
  Eval eval = EvalTwoEmptiesOrMin(x1, x2, player, opponent, upper, n_visited);
  if (eval > kLessThenMinEval) {
    return eval;
  }
  eval = EvalTwoEmptiesOrMin(x1, x2, opponent, player, -lower, n_visited);
  if (eval > kLessThenMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

forceinline(Eval EvalThreeEmptiesOrMin(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited) noexcept);

inline Eval EvalThreeEmptiesOrMin(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited) noexcept {
  (*n_visited)++;
  Eval eval = kLessThenMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalTwoEmpties(x2, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -lower, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = MaxEval(eval, -EvalTwoEmpties(x1, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -MaxEval(lower, eval), n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    return MaxEval(eval, -EvalTwoEmpties(x1, x2, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -MaxEval(lower, eval), n_visited));
  }
  return eval;
}

forceinline(Eval EvalThreeEmpties(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited) noexcept);

inline Eval EvalThreeEmpties(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited) noexcept {
  Eval eval = EvalThreeEmptiesOrMin(x1, x2, x3, player, opponent, lower, upper, n_visited);
  if (eval > kLessThenMinEval) {
    return eval;
  }
  eval = EvalThreeEmptiesOrMin(x1, x2, x3, opponent, player, -upper, -lower, n_visited);
  if (eval > kLessThenMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

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

#endif /* EVALUATOR_LAST_MOVES_H */

