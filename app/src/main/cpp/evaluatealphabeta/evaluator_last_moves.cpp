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

#include <cassert>
#include <cstring>

#include "../board/get_flip.h"
#include "evaluator_last_moves.h"
#include "../board/stable.h"
#include "../board/board.h"

constexpr BitPattern kCentralPattern = ParsePattern(
    "--------"
    "--XXXX--"
    "-XXXXXX-"
    "-XXXXXX-"
    "-XXXXXX-"
    "-XXXXXX-"
    "--XXXX--"
    "--------");

constexpr BitPattern kEdgePattern = ParsePattern(
    "--XXXX--"
    "--------"
    "X------X"
    "X------X"
    "X------X"
    "X------X"
    "--------"
    "--XXXX--");

constexpr BitPattern kXPattern = ParsePattern(
    "--------"
    "-X----X-"
    "--------"
    "--------"
    "--------"
    "--------"
    "-X----X-"
    "--------");

constexpr BitPattern kXCPattern = ParsePattern(
    "-X----X-"
    "XX----XX"
    "--------"
    "--------"
    "--------"
    "--------"
    "XX----XX"
    "-X----X-");

constexpr BitPattern kSpace0Pattern = ParsePattern(
    "--------"
    "--------"
    "--------"
    "--------"
    "----XXXX"
    "----XXXX"
    "----XXXX"
    "----XXXX");
constexpr BitPattern kSpace1Pattern = kSpace0Pattern << 4;
constexpr BitPattern kSpace2Pattern = kSpace0Pattern << 32;
constexpr BitPattern kSpace3Pattern = kSpace0Pattern << 36;

Eval EvalTwoEmptiesOrMin(const Square x1, const Square x2, const BitPattern player, const BitPattern opponent, const Eval upper, int* const n_visited) noexcept {
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
    return max(eval, -EvalOneEmpty(x1, NewPlayer(flip, opponent), NewOpponent(flip, player)));
  }
  return eval;
}

Eval EvalTwoEmpties(const Square x1, const Square x2, const BitPattern player, const BitPattern opponent, const Eval lower, const Eval upper, int* const n_visited) noexcept {
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

Eval EvalThreeEmptiesOrMin(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited) {
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
    eval = max(eval, -EvalTwoEmpties(x1, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    return max(eval, -EvalTwoEmpties(x1, x2, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
  }
  return eval;
}

Eval EvalThreeEmpties(
  const Square x1, const Square x2, const Square x3,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper,
  int* const n_visited) {
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

Eval EvalFourEmptiesOrMin(
  const Square x1, const Square x2, const Square x3, const Square x4,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper, const bool swap,
  int* const n_visited) {
  (*n_visited)++;
  Eval eval = kLessThenMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalThreeEmpties(x2, x3, x4, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -lower, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = max(eval, -EvalThreeEmpties(x1, x3, x4, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = max(eval, -EvalThreeEmpties(x4, x1, x2, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
    } else {
      eval = max(eval, -EvalThreeEmpties(x1, x2, x4, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
    }
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x4, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = max(eval, -EvalThreeEmpties(x3, x1, x2, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
    } else {
      eval = max(eval, -EvalThreeEmpties(x1, x2, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), n_visited));
    }
  }
  return eval;
}

Eval EvalFourEmpties(
  const Square x1, const Square x2, const Square x3, const Square x4,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper, const bool swap,
  const BitPattern last_flip, const BitPattern stable,
  int* const n_visited) {
  if (GetUpperBoundFromStable(stable, opponent) - ((last_flip & kCornerPattern) == 0 ? 10 : 20) <= lower) {
    Eval stability_cutoff_upper = GetUpperBoundFromStable(GetStableDisks(opponent, player, stable), opponent);
    if (stability_cutoff_upper <= lower) {
      return stability_cutoff_upper;
    }
  }
  Eval eval = EvalFourEmptiesOrMin(x1, x2, x3, x4, player, opponent, lower, upper, swap, n_visited);
  if (eval > kLessThenMinEval) {
    return eval;
  }
  eval = EvalFourEmptiesOrMin(x1, x2, x3, x4, opponent, player, -upper, -lower, swap, n_visited);
  if (eval > kLessThenMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

Eval EvalFiveEmptiesOrMin(
  const Square x1, const Square x2, const Square x3, const Square x4, const Square x5,
  const BitPattern player, const BitPattern opponent,
  const Eval lower, const Eval upper, const bool swap,
  const BitPattern stable,
  int* const n_visited) {
  (*n_visited)++;
  Eval eval = kLessThenMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalFourEmpties(x2, x3, x4, x5, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -lower, swap, flip, stable, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = max(eval, -EvalFourEmpties(x1, x3, x4, x5, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), flip, swap, stable, n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    eval = max(eval, -EvalFourEmpties(x1, x2, x4, x5, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), flip, swap, stable, n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x4, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = max(eval, -EvalFourEmpties(x5, x1, x2, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), flip, swap, stable, n_visited));
    } else {
      eval = max(eval, -EvalFourEmpties(x1, x2, x3, x5, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), flip, swap, stable, n_visited));
    }
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x5, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = max(eval, -EvalFourEmpties(x4, x1, x2, x3, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), flip, swap, stable, n_visited));
    } else {
      eval = max(eval, -EvalFourEmpties(x1, x2, x3, x4, NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, -max(lower, eval), flip, swap, stable, n_visited));
    }
  }
  return eval;
}

Eval EvalFiveEmpties(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    const BitPattern last_flip, const BitPattern stable,
    int* const n_visited) {
  
  if (GetUpperBoundFromStable(stable, opponent) - ((last_flip & kCornerPattern) == 0 ? 14 : 20) <= lower) {
    Eval stability_cutoff_upper = GetUpperBoundFromStable(GetStableDisks(opponent, player, stable), opponent);
    if (stability_cutoff_upper <= lower) {
      return stability_cutoff_upper;
    }
  }
  BitPattern empties = ~(player | opponent);
  Square x[5];
  Square space_3[3];
  int new_cont_x = 0;
  int cont_x = 0;
  Square move;
  bool has_space_2 = false;
  bool has_space_3 = false;
  for (BitPattern space : {kSpace0Pattern, kSpace1Pattern, kSpace2Pattern, kSpace3Pattern}) {
    int space_size = __builtin_popcountll(space & empties);
    if (space_size == 0) {
      continue;
    } else if (space_size == 1) {
      move = __builtin_ctzll(space & empties);
      x[cont_x++] = move;
      empties = empties & ~(1L << move);
      continue;
    } else if (space_size == 2) {  // Can be 1:1:1:2, 1:2:2, 3:2.
      new_cont_x = cont_x;
      cont_x = has_space_2 ? 1 : 3;
      has_space_2 = true;
    } else if (space_size == 3) {  // Can be 2:3 or 1:1:3.
      has_space_3 = true;
      int j = 0;
      for (BitPattern mask : {kCornerPattern, kCentralPattern, kEdgePattern, kXCPattern}) {
        mask = mask & space;
        while ((mask & empties) != 0) {
          move = __builtin_ctzll(mask & empties);
          space_3[j++] = move;
          empties = empties & (~(1ULL << move));
        }
      }
      continue;
    } else if (space_size == 4) {  // Must be 1:4.
      new_cont_x = 0;
      cont_x = 1;
    }// else if (space_size == 5) do nothing.
    for (BitPattern mask : {kCornerPattern, kCentralPattern, kEdgePattern, kXCPattern}) {
      mask = mask & space;
      while ((mask & empties) != 0) {
        move = __builtin_ctzll(mask & empties);
        x[cont_x++] = move;
        empties = empties & (~(1ULL << move));
      }
    }
    cont_x = new_cont_x;
  }
  if (has_space_3) {
    if (has_space_2) {
      std::memcpy(x, space_3, sizeof(Square) * 3);
    } else {
      std::memcpy(x + 2, space_3, sizeof(Square) * 3);
    }
  }
  
  Eval eval = EvalFiveEmptiesOrMin(x[0], x[1], x[2], x[3], x[4], player, opponent, lower, upper, has_space_2, stable, n_visited);
  if (eval > kLessThenMinEval) {
    return eval;
  }
  eval = EvalFiveEmptiesOrMin(x[0], x[1], x[2], x[3], x[4], opponent, player, -upper, -lower, has_space_2, stable, n_visited);
  if (eval > kLessThenMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}
