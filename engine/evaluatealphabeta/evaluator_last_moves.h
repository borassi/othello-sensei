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

forceinline(int EvalOneEmpty(Square x, BitPattern player, BitPattern opponent) noexcept);
inline int EvalOneEmpty(Square x, BitPattern player, BitPattern opponent) noexcept {
  BitPattern flip = GetFlip(x, player, opponent);
  if (__builtin_expect(flip != 0, true)) {
    return (Eval) ((__builtin_popcountll(NewOpponent(flip, player)) * 2) - 64);
  }
  flip = GetFlip(x, opponent, player);
  if (flip) {
    return (Eval) (64 - (__builtin_popcountll(opponent | flip) * 2));
  }
  int playerDisks = (int) __builtin_popcountll(player) * 2;
  return (Eval) (playerDisks - (playerDisks >= 64 ? 62 : 64));
}

forceinline(int EvalTwoEmptiesOrMin(
    const Square x1, const Square x2, const BitPattern player,
    const BitPattern opponent, const int upper, int* const n_visited) noexcept);

inline int EvalTwoEmptiesOrMin(
    const Square x1, const Square x2, const BitPattern player,
    const BitPattern opponent, const int upper, int* const n_visited) noexcept {
  (*n_visited)++;
  int eval = -66;
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
    return std::max(eval, -EvalOneEmpty(x1, NewPlayer(flip, opponent), NewOpponent(flip, player)));
  }
  return eval;
}

forceinline(int EvalTwoEmpties(
  const Square x1, const Square x2, const BitPattern player,
  const BitPattern opponent, const int lower, const int upper,
  int* const n_visited) noexcept);

inline int EvalTwoEmpties(
  const Square x1, const Square x2, const BitPattern player,
  const BitPattern opponent, const int lower, const int upper,
  int* const n_visited) noexcept {
  int eval = EvalTwoEmptiesOrMin(x1, x2, player, opponent, upper, n_visited);
  if (eval > kLessThenMinEval) {
    return eval;
  }
  eval = EvalTwoEmptiesOrMin(x1, x2, opponent, player, -lower, n_visited);
  if (eval > kLessThenMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

forceinline(int EvalThreeEmptiesOrMin(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const int lower, const int upper,
  int* const n_visited) noexcept);

inline int EvalThreeEmptiesOrMin(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const int lower, const int upper,
  int* const n_visited) noexcept {
  (*n_visited)++;
  int eval = -66;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalTwoEmpties(x2, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -lower, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = std::max(eval, -EvalTwoEmpties(x1, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -std::max(lower, eval), n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    return std::max(eval, -EvalTwoEmpties(x1, x2, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -std::max(lower, eval), n_visited));
  }
  return eval;
}

forceinline(int EvalThreeEmpties(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const int lower, const int upper,
  int* const n_visited) noexcept);

inline int EvalThreeEmpties(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const int lower, const int upper,
  int* const n_visited) noexcept {
  int eval = EvalThreeEmptiesOrMin(x1, x2, x3, player, opponent, lower, upper, n_visited);
  if (eval > kLessThenMinEval) {
    return eval;
  }
  eval = EvalThreeEmptiesOrMin(x1, x2, x3, opponent, player, -upper, -lower, n_visited);
  if (eval > kLessThenMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

int EvalFourEmpties(
  Square x1, Square x2, Square x3, Square x4,
  BitPattern player, BitPattern opponent,
  int lower, int upper, bool swap,
  BitPattern last_flip, BitPattern stable,
  int* n_visited);
int EvalFiveEmpties(
    BitPattern player, BitPattern opponent,
    int lower, int upper,
    BitPattern last_flip, BitPattern stable,
    int* n_visited);

inline std::vector<Square> GetAllEmpties(BitPattern player, BitPattern opponent) {
  std::vector<Square> result;
  BitPattern empties = ~(player | opponent);
  FOR_EACH_SET_BIT(empties, empty) {
    result.push_back((Square) __builtin_ctzll(empty));
  }
  return result;
}

#endif /* EVALUATOR_LAST_MOVES_H */

