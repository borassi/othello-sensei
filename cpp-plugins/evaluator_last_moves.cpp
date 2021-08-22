/*
 * Copyright 2021 Google LLC
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

#include <assert.h>
#include <cstring>

#include "get_flip.h"
#include "evaluator_last_moves.h"
#include "stable.h"
#include "board.h"

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

Eval EvalTwoEmptiesOrMin(const Move x1, const Move x2, const BitPattern player, const BitPattern opponent, const Eval upper, int* const n_visited) noexcept {
  (*n_visited)++;
  Eval eval = kMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    (*n_visited)++;
    eval = -EvalOneEmpty(x2, opponent & ~flip, player | flip);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    (*n_visited)++;
    return std::max(eval, -EvalOneEmpty(x1, opponent & ~flip, player | flip));
  }
  return eval;
}

Eval EvalTwoEmpties(const Move x1, const Move x2, const BitPattern player, const BitPattern opponent, const Eval lower, const Eval upper, int* const n_visited) noexcept {
  Eval eval = EvalTwoEmptiesOrMin(x1, x2, player, opponent, upper, n_visited);
  if (eval > kMinEval) {
    return eval;
  }
  eval = EvalTwoEmptiesOrMin(x1, x2, opponent, player, -lower, n_visited);
  if (eval > kMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

Eval EvalThreeEmptiesOrMin(
    const Move x1, const Move x2, const Move x3,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    int* const n_visited) {
  (*n_visited)++;
  Eval eval = kMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalTwoEmpties(x2, x3, opponent & ~flip, player | flip, -upper, -lower, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = std::max(eval, -EvalTwoEmpties(x1, x3, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    return std::max(eval, -EvalTwoEmpties(x1, x2, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));
  }
  return eval;
}

Eval EvalThreeEmpties(
    const Move x1, const Move x2, const Move x3,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    int* const n_visited) {
  Eval eval = EvalThreeEmptiesOrMin(x1, x2, x3, player, opponent, lower, upper, n_visited);
  if (eval > kMinEval) {
    return eval;
  }
  eval = EvalThreeEmptiesOrMin(x1, x2, x3, opponent, player, -upper, -lower, n_visited);
  if (eval > kMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

Eval EvalFourEmptiesOrMin(
    const Move x1, const Move x2, const Move x3, const Move x4,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, const bool swap,
    int* const n_visited) {
  (*n_visited)++;
  Eval eval = kMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalThreeEmpties(x2, x3, x4, opponent & ~flip, player | flip, -upper, -lower, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = std::max(eval, -EvalThreeEmpties(x1, x3, x4, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = std::max(eval, -EvalThreeEmpties(x4, x1, x2, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));      
    } else {
      eval = std::max(eval, -EvalThreeEmpties(x1, x2, x4, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));
    }
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x4, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = std::max(eval, -EvalThreeEmpties(x3, x1, x2, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));      
    } else {
      eval = std::max(eval, -EvalThreeEmpties(x1, x2, x3, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), n_visited));
    }
  }
  return eval;
}

Eval EvalFourEmpties(
    const Move x1, const Move x2, const Move x3, const Move x4,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, const bool swap,
    const BitPattern last_flip, const BitPattern stable, 
    int* const n_visited) {
  if (GetUpperBoundFromStable(stable, opponent) - ((last_flip & kCornerPattern) == 0 ? 8 : 20) <= lower) {
    Eval stability_cutoff_upper = GetUpperBoundFromStable(GetStableDisks(opponent, player, stable), opponent);
    if (stability_cutoff_upper <= lower) {
      return stability_cutoff_upper;
    }
  }
  Eval eval = EvalFourEmptiesOrMin(x1, x2, x3, x4, player, opponent, lower, upper, swap, n_visited);
  if (eval > kMinEval) {
    return eval;
  }
  eval = EvalFourEmptiesOrMin(x1, x2, x3, x4, opponent, player, -upper, -lower, swap, n_visited);
  if (eval > kMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

Eval EvalFiveEmptiesOrMin(
    const Move x1, const Move x2, const Move x3, const Move x4, const Move x5,
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, const bool swap,
    const BitPattern stable,
    int* const n_visited) {
  (*n_visited)++;
  Eval eval = kMinEval;
  BitPattern flip = GetFlip(x1, player, opponent);
  if (flip != 0) {
    eval = -EvalFourEmpties(x2, x3, x4, x5, opponent & ~flip, player | flip, -upper, -lower, swap, flip, stable, n_visited);
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x2, player, opponent);
  if (flip != 0) {
    eval = std::max(eval, -EvalFourEmpties(x1, x3, x4, x5, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), flip, swap, stable, n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x3, player, opponent);
  if (flip != 0) {
    eval = std::max(eval, -EvalFourEmpties(x1, x2, x4, x5, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), flip, swap, stable, n_visited));
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x4, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = std::max(eval, -EvalFourEmpties(x5, x1, x2, x3, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), flip, swap, stable, n_visited));      
    } else {
      eval = std::max(eval, -EvalFourEmpties(x1, x2, x3, x5, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), flip, swap, stable, n_visited));
    }
    if (eval >= upper) {
      return eval;
    }
  }
  flip = GetFlip(x5, player, opponent);
  if (flip != 0) {
    if (swap) {
      eval = std::max(eval, -EvalFourEmpties(x4, x1, x2, x3, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), flip, swap, stable, n_visited));      
    } else {
      eval = std::max(eval, -EvalFourEmpties(x1, x2, x3, x4, opponent & ~flip, player | flip, -upper, -std::max(lower, eval), flip, swap, stable, n_visited));
    }
  }
  return eval;
}

Eval EvalFiveEmpties(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper,
    const BitPattern last_flip, const BitPattern stable,
    int* const n_visited) {
  
  if (GetUpperBoundFromStable(stable, opponent) - ((last_flip & kCornerPattern) == 0 ? 14 : 18) <= lower) {
    Eval stability_cutoff_upper = GetUpperBoundFromStable(GetStableDisks(opponent, player, stable), opponent);
    if (stability_cutoff_upper <= lower) {
      return stability_cutoff_upper;
    }
  }
  // TODO: stable disks.
  long empties = ~(player | opponent);
  Move x[5];
  Move space_3[3];
  int new_cont_x = 0;
  int cont_x = 0;
  Move move;
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
      std::memcpy(x, space_3, sizeof(Move) * 3);
    } else {
      std::memcpy(x + 2, space_3, sizeof(Move) * 3);
    }
  }
  
  Eval eval = EvalFiveEmptiesOrMin(x[0], x[1], x[2], x[3], x[4], player, opponent, lower, upper, has_space_2, stable, n_visited);
  if (eval > kMinEval) {
    return eval;
  }
  eval = EvalFiveEmptiesOrMin(x[0], x[1], x[2], x[3], x[4], opponent, player, -upper, -lower, has_space_2, stable, n_visited);
  if (eval > kMinEval) {
    return -eval;
  }
  return GetEvaluationGameOver(player, opponent);
}

int Evaluate(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, bool passed,
    const BitPattern last_flip, const BitPattern stable,
    int* const n_visited) {
  (*n_visited)++;
  
  BitPattern new_stable = stable;
  new_stable = GetStableDisks(opponent, player, new_stable);
  Eval stability_cutoff_upper = GetUpperBoundFromStable(new_stable, opponent);
  if (stability_cutoff_upper <= lower) {
    return stability_cutoff_upper;
  }
  
  BitPattern empties = ~(player | opponent);
  int n_empties = __builtin_popcountll(empties);
  Move move;
  Eval eval = kMinEval;
  BitPattern flip;
  BitPattern move_bit;
  BitPattern neighbors_player = Neighbors(player);
  BitPattern empties_neighbors_opponent = Neighbors(opponent) & empties;

  for (BitPattern mask : {
      ~Neighbors(empties) & neighbors_player,
      UniqueInEdges(empties) & neighbors_player,
      Neighbors(last_flip) & kCornerPattern,
      kCornerPattern,
      FirstLastInEdges(empties),
      ((last_flip & kXPattern) != 0) ? Neighbors(last_flip) : 0,
      kCentralPattern,
      kEdgePattern,
      kXCPattern}) {
    while ((mask & empties_neighbors_opponent) != 0) {
      move = __builtin_ctzll(mask & empties_neighbors_opponent);
      move_bit = 1ULL << move;
      empties_neighbors_opponent = empties_neighbors_opponent & (~move_bit);
      flip = GetFlip(move, player, opponent);
      if (flip == 0) {
        continue;
      }
      if (n_empties == 6) {
        eval = std::max(
            eval, 
            -EvalFiveEmpties(
                opponent & ~flip, player | flip, -upper, -std::max(lower, eval),
                flip, new_stable, n_visited));
      } else {
        eval = std::max(
            eval, 
            -Evaluate(
                opponent & ~flip, player | flip, -upper, -std::max(lower, eval),
                false, flip, new_stable, n_visited));        
      }
      if (eval >= upper) {
        break;
      }
    }
    if (eval >= upper) {
      break;
    }
  }
  if (eval == kMinEval) {
    if (passed) {
      return GetEvaluationGameOver(player, opponent);
    }
    return -Evaluate(opponent, player, -upper, -lower, true, last_flip, new_stable, n_visited);
  }
  return eval;
}

int Evaluate(
    const BitPattern player, const BitPattern opponent,
    const Eval lower, const Eval upper, int* const n_visited) {
  return Evaluate(player, opponent, lower, upper, false, 0, 0, n_visited);
}

JNIEXPORT jobject JNICALL Java_evaluateposition_EvaluatorLastMoves_evaluateCPPInternal(
    JNIEnv* env, jclass java_class, jlong player, jlong opponent, jint lower,
    jint upper) {
//  return lower + upper;
//  int n_visited = 0;
//  return Evaluate(player, opponent, lower, upper, &n_visited);
  int n_visited = 0;
  int eval = Evaluate(player, opponent, lower, upper, &n_visited);
  jclass EvalWithVisited = env->FindClass("evaluateposition/EvalWithVisited");
  return env->NewObject(EvalWithVisited, env->GetMethodID(EvalWithVisited, "<init>", "(IJ)V"), eval, n_visited);
}