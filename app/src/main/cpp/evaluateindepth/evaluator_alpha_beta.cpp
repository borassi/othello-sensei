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

#include "../board/bitpattern.h"
#include "../board/stable.h"
#include "evaluator_alpha_beta.h"
#include "evaluator_last_moves.h"

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

constexpr int NextNEmpties(int n_empties) {
  return std::min(64, std::max(0, n_empties - 1));
}

EvaluatorAlphaBeta::EvaluateInternalFunction
    EvaluatorAlphaBeta::evaluators_[65] = {
    &EvaluatorAlphaBeta::EvaluateInternal<0, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<1, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<2, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<3, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<4, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<5, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<6, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<7, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<8, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<9, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<10, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<11, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<12, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<13, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<14, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<15, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<16, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<17, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<18, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<19, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<20, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<21, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<22, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<23, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<24, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<25, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<26, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<27, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<28, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<29, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<30, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<31, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<32, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<33, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<34, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<35, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<36, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<37, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<38, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<39, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<40, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<41, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<42, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<43, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<44, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<45, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<46, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<47, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<48, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<49, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<50, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<51, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<52, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<53, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<54, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<55, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<56, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<57, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<58, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<59, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<60, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<61, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<62, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<63, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<64, false>,
};

// clangtidy: no-warning.
template<int depth, bool passed>
EvalLarge EvaluatorAlphaBeta::EvaluateInternal(
    const BitPattern player, const BitPattern opponent,
    const EvalLarge lower, const EvalLarge upper,
    const BitPattern last_flip, const BitPattern stable) {
  n_visited_++;
  assert(kMinEvalLarge <= lower && lower < kMaxEvalLarge);
  assert(kMinEvalLarge < upper && upper <= kMaxEvalLarge);

  BitPattern new_stable = stable;
  new_stable = GetStableDisks(opponent, player, new_stable);
  EvalLarge stability_cutoff_upper = EvalToEvalLarge(GetUpperBoundFromStable(new_stable, opponent));
  if (stability_cutoff_upper <= lower) {
    return stability_cutoff_upper;
  }

  if (depth >= kMinEmptiesForHashMap) {
    std::pair<Eval, Eval> hash_eval =
        hash_map_->GetLowerUpper(player, opponent, depth);
    if (hash_eval.first >= upper || hash_eval.first == hash_eval.second) {
      return hash_eval.first;
    } else if (hash_eval.second <= lower) {
      return hash_eval.second;
    }
  }
  Square move;
  EvalLarge eval = kLessThenMinEvalLarge;
  BitPattern flip;
  BitPattern move_bit;
  BitPattern empties = ~(player | opponent);
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
      if (depth == 6) {
        int cur_n_visited = 0;
        eval = std::max(
            eval,
            -EvalToEvalLarge(EvalFiveEmpties(
                NewPlayer(flip, opponent), NewOpponent(flip, player),
                (Eval) ((-upper - 1024) / 8 + 128),
                (Eval) ((-(std::max(lower, eval)) + 1024) / 8 - 128),
                flip, new_stable, &cur_n_visited)));
        n_visited_ += cur_n_visited;
      } else {
        eval = std::max(
            eval,
            -EvaluateInternal<NextNEmpties(depth), false>(
                NewPlayer(flip, opponent), NewOpponent(flip, player),
                    -upper, -std::max(lower, eval), flip, new_stable));
      }
      if (eval >= upper) {
        break;
      }
    }
    if (eval >= upper) {
      break;
    }
  }
  if (eval == kLessThenMinEvalLarge) {
    if (passed) {
      return EvalToEvalLarge(GetEvaluationGameOver(player, opponent));
    }
    return -EvaluateInternal<depth, true>(opponent, player, -upper, -lower, last_flip, new_stable);
  } else if (depth >= kMinEmptiesForHashMap) {
    hash_map_
        ->Update(player, opponent, epoch_, depth, eval, lower, upper, 0, 0);
  }
  return eval;
}
