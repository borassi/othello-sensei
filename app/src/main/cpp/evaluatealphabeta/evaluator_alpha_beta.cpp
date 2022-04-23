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
#include "../board/board.h"
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

MoveIteratorQuick::MoveIteratorQuick() : masks_(), player_(0), opponent_(0), candidate_moves_(0), current_mask_(0) {
  masks_[3] = kCornerPattern;
  masks_[6] = kCentralPattern;
  masks_[7] = kEdgePattern;
  masks_[8] = ~0ULL;
}

void MoveIteratorQuick::Setup(BitPattern player, BitPattern opponent,
                              BitPattern last_flip,
                              EvaluatorDepthOneBase* evaluator_depth_one) {
  player_ = player;
  opponent_ = opponent;
  BitPattern empties = ~(player | opponent);
  candidate_moves_ = Neighbors(opponent) & empties;
  current_mask_ = 0;
  BitPattern neighbors_player = Neighbors(player);
  masks_[0] = ~Neighbors(empties) & neighbors_player;
  masks_[1] = UniqueInEdges(empties) & neighbors_player;
  masks_[2] = Neighbors(last_flip) & kCornerPattern;
  masks_[4] = FirstLastInEdges(empties);
  masks_[5] = ((last_flip & kXPattern) != 0) ? Neighbors(last_flip) : 0;
}

BitPattern MoveIteratorQuick::NextFlip() {
  BitPattern mask = masks_[current_mask_];
  BitPattern flip = 0;
  while (flip == 0 && candidate_moves_ != 0) {
    while ((mask & candidate_moves_) == 0) {
      ++current_mask_;
      mask = masks_[current_mask_];
    }
    int move = __builtin_ctzll(mask & candidate_moves_);
    candidate_moves_ &= (~(1ULL << move));
    flip = GetFlip(move, player_, opponent_);
  }
  return flip;
}

void MoveIteratorEval::Setup(
    BitPattern player, BitPattern opponent, BitPattern last_flip,
    EvaluatorDepthOneBase* evaluator_depth_one_base) {
  BitPattern empties = ~(player | opponent);
  BitPattern candidate_moves = Neighbors(opponent) & empties;
  BitPattern flip;
  remaining_moves_ = 0;
  FOR_EACH_SET_BIT(candidate_moves, square_pattern) {
    Square square = __builtin_ctzll(square_pattern);
    flip = GetFlip(square, player, opponent);
    if (flip == 0) {
      continue;
    }
//    evaluator_depth_one_base->Update(square_pattern, flip);
    int value = 0;//-evaluator_depth_one_base->Evaluate();
    BitPattern moves = GetMoves(NewPlayer(flip, opponent), NewOpponent(flip, player));
    value = -(__builtin_popcountll(moves) + __builtin_popcountll(moves & kCornerPattern)) * 1000;
    value += kSquareValue[square];
    moves_[remaining_moves_].Set(flip, value);
//    evaluator_depth_one_base->UndoUpdate(square_pattern, flip);
    remaining_moves_++;
  }
}

BitPattern MoveIteratorEval::NextFlip() {
  if (remaining_moves_ == 0) {
    return 0;
  }
  Move* best_move = moves_;
  for (int i = 0; i < remaining_moves_; ++i) {
    if (moves_[i].GetValue() > best_move->GetValue()) {
      best_move = moves_ + i;
    }
  }
  BitPattern flip = best_move->GetFlip();
  --remaining_moves_;
  *best_move = moves_[remaining_moves_];
  return flip;
}

EvaluatorAlphaBeta::EvaluateInternalFunction
    EvaluatorAlphaBeta::solvers_[kMaxDepth] =  {
    &EvaluatorAlphaBeta::EvaluateInternal<0, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<1, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<2, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<3, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<4, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<5, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<6, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<7, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<8, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<9, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<10, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<11, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<12, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<13, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<14, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<15, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<16, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<17, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<18, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<19, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<20, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<21, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<22, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<23, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<24, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<25, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<26, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<27, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<28, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<29, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<30, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<31, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<32, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<33, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<34, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<35, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<36, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<37, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<38, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<39, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<40, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<41, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<42, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<43, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<44, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<45, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<46, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<47, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<48, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<49, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<50, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<51, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<52, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<53, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<54, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<55, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<56, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<57, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<58, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<59, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<60, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<61, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<62, false, true>,
    &EvaluatorAlphaBeta::EvaluateInternal<63, false, true>
};
EvaluatorAlphaBeta::EvaluateInternalFunction
    EvaluatorAlphaBeta::evaluators_[kMaxDepth] = {
    &EvaluatorAlphaBeta::EvaluateInternal<0, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<1, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<2, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<3, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<4, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<5, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<6, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<7, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<8, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<9, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<10, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<11, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<12, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<13, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<14, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<15, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<16, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<17, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<18, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<19, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<20, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<21, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<22, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<23, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<24, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<25, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<26, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<27, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<28, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<29, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<30, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<31, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<32, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<33, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<34, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<35, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<36, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<37, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<38, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<39, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<40, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<41, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<42, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<43, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<44, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<45, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<46, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<47, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<48, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<49, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<50, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<51, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<52, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<53, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<54, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<55, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<56, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<57, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<58, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<59, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<60, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<61, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<62, false, false>,
    &EvaluatorAlphaBeta::EvaluateInternal<63, false, false>
};

constexpr bool UseMoveIteratorQuick(int depth, bool solve) {
  return (solve && depth < 11) || (!solve && depth < 3);
}

constexpr bool UpdateDepthOneEvaluator(int depth, bool solve) {
  return !solve || !UseMoveIteratorQuick(depth, solve);
}

constexpr bool UseHashMap(int depth, bool solve) {
  return (solve && depth >= kMinEmptiesForHashMap)
         || (!solve && depth >= kMinDepthForHashMap);
}

EvaluatorAlphaBeta::EvaluatorAlphaBeta(
    HashMap* hash_map,
    EvaluatorFactory evaluator_depth_one_factory) :
    n_visited_(0),
    epoch_(0),
    hash_map_(hash_map),
    evaluator_depth_one_(evaluator_depth_one_factory()) {
  for (int i = 0; i < 64; ++i) {
    for (bool solve : {true, false}) {
      int offset = MoveIteratorOffset(i, solve);
      if (UseMoveIteratorQuick(i, solve)) {
        move_iterators_[offset] = std::make_unique<MoveIteratorQuick>();
      } else {
        move_iterators_[offset] = std::make_unique<MoveIteratorEval>();
      }
    }
  }
}

// clangtidy: no-warning.
template<int depth, bool passed, bool solve>
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

  if (UseHashMap(depth, solve)) {
    std::pair<EvalLarge, EvalLarge> hash_eval =
        hash_map_->GetLowerUpper(player, opponent, depth);
    if (hash_eval.first >= upper || hash_eval.first == hash_eval.second) {
      return hash_eval.first;
    } else if (hash_eval.second <= lower) {
      return hash_eval.second;
    }
  }
  EvalLarge depth_zero_eval;
  if (UpdateDepthOneEvaluator(depth, solve)) {
    if (depth == 1) {
      depth_zero_eval = evaluator_depth_one_->Evaluate();
    }
    evaluator_depth_one_->Invert();
  }
  BitPattern square;
  EvalLarge eval = kLessThenMinEvalLarge;
  int cur_n_visited;
  MoveIteratorBase* moves =
      move_iterators_[MoveIteratorOffset(depth, solve)].get();
  moves->Setup(player, opponent, last_flip, evaluator_depth_one_.get());
  for (BitPattern flip = moves->NextFlip(); flip != 0; flip = moves->NextFlip()) {
    if (UpdateDepthOneEvaluator(depth, solve)) {
      square = SquareFromFlip(flip, player, opponent);
      evaluator_depth_one_->Update(square, flip);
    }
    if (depth == 6 && solve) {
      cur_n_visited = 0;
      eval = std::max(
          eval,
          -EvalToEvalLarge(EvalFiveEmpties(
              NewPlayer(flip, opponent), NewOpponent(flip, player),
              (Eval) ((-upper - 1024) / 8 + 128),
              (Eval) ((-(std::max(lower, eval)) + 1024) / 8 - 128),
              flip, new_stable, &cur_n_visited)));
      n_visited_ += cur_n_visited;
    } else if (!solve && depth == 1) {
      eval = std::max(
          eval,
          (depth_zero_eval * kWeightDepthZero - evaluator_depth_one_->Evaluate() * kWeightDepthOne) / (kWeightDepthZero + kWeightDepthOne));
      ++n_visited_;
    } else {
      eval = std::max(
          eval,
          -EvaluateInternal<NextNEmpties(depth), false, solve>(
              NewPlayer(flip, opponent), NewOpponent(flip, player),
                  -upper, -std::max(lower, eval), flip, new_stable));
    }
    if (UpdateDepthOneEvaluator(depth, solve)) {
      evaluator_depth_one_->UndoUpdate(square, flip);
    }
    if (eval >= upper) {
      break;
    }
  }
  if (eval == kLessThenMinEvalLarge) {
    if (passed) {
      eval = EvalToEvalLarge(GetEvaluationGameOver(player, opponent));
    } else {
      eval = -EvaluateInternal<depth, true, solve>(
          opponent, player, -upper, -lower, last_flip, new_stable);
    }
  } else if (UseHashMap(depth, solve)) {
    hash_map_
        ->Update(player, opponent, epoch_, depth, eval, lower, upper, 0, 0);
  }
  if (!solve) {
    evaluator_depth_one_->Invert();
  }
  return eval;
}
