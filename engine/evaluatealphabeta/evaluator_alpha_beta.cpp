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

#include "../board/bitpattern.h"
#include "../board/board.h"
#include "../board/get_moves.h"
#include "../board/stable.h"
#include "../estimators/win_probability.h"
#include "../estimators/endgame_time.h"
#include "evaluator_alpha_beta.h"
#include "evaluator_last_moves.h"

constexpr int kMinEmptiesForDisproofNumber = 12;

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

constexpr int NextNEmpties(int n_empties) {
  return std::min(64, std::max(0, n_empties - 1));
}

constexpr int MoveIteratorOffset(int depth, bool solve, bool unlikely) {
  return depth + (solve ? EvaluatorAlphaBeta::kMaxDepth : 0) + (unlikely ? 2 * EvaluatorAlphaBeta::kMaxDepth : 0);
}

void MoveIteratorVeryQuick::Setup(
    BitPattern player, BitPattern opponent,
    BitPattern last_flip, int upper, HashMapEntry* const entry,
    EvaluatorDepthOneBase* evaluator_depth_one) {
  player_ = player;
  opponent_ = opponent;
  candidate_moves_ = Neighbors(opponent) & ~(player | opponent);
}

BitPattern MoveIteratorVeryQuick::NextFlip() {
  BitPattern flip = 0;
  while (flip == 0 && candidate_moves_ != 0) {
    auto move = (Square) __builtin_ctzll(candidate_moves_);
    candidate_moves_ &= (~(1ULL << move));
    assert(((1ULL << move) & (player_ | opponent_)) == 0);
    flip = GetFlip(move, player_, opponent_);
  }
  if (flip != 0) {
    stats_->Add(1, VERY_QUICK);
  }
  return flip;
}

template<bool very_quick>
MoveIteratorQuick<very_quick>::MoveIteratorQuick(Stats* stats) :
    MoveIteratorBase(stats),
    player_(),
    opponent_(),
    candidate_moves_(),
    masks_(),
    current_mask_() {
  if (very_quick) {
    masks_[1] = kCornerPattern;
    masks_[3] = kCentralPattern;
    masks_[4] = kEdgePattern;
    masks_[5] = ~0ULL;
  } else {
    masks_[3] = kCornerPattern;
    masks_[6] = kCentralPattern;
    masks_[7] = kEdgePattern;
    masks_[8] = ~0ULL;
  }
}

template<bool very_quick>
void MoveIteratorQuick<very_quick>::Setup(
    BitPattern player, BitPattern opponent,
    BitPattern last_flip, int upper, HashMapEntry* const entry,
    EvaluatorDepthOneBase* evaluator_depth_one) {
  player_ = player;
  opponent_ = opponent;
  BitPattern empties = ~(player | opponent);
  candidate_moves_ = Neighbors(opponent) & empties;
  current_mask_ = 0;
  BitPattern neighbors_player = Neighbors(player);
  if (very_quick) {
    masks_[0] = ~Neighbors(empties) & neighbors_player;
    masks_[2] = ((last_flip & kXPattern) != 0) ? Neighbors(last_flip) : 0;
  } else {
    masks_[0] = ~Neighbors(empties) & neighbors_player;
    masks_[1] = UniqueInEdges(empties) & neighbors_player;
    masks_[2] = Neighbors(last_flip) & kCornerPattern;
    masks_[4] = FirstLastInEdges(empties);
    masks_[5] = ((last_flip & kXPattern) != 0) ? Neighbors(last_flip) : 0;
  }
}

template<bool very_quick>
BitPattern MoveIteratorQuick<very_quick>::NextFlip() {
  BitPattern mask = masks_[current_mask_];
  BitPattern flip = 0;
  while (flip == 0 && candidate_moves_ != 0) {
    while ((mask & candidate_moves_) == 0) {
      ++current_mask_;
      mask = masks_[current_mask_];
    }
    auto move = (Square) __builtin_ctzll(mask & candidate_moves_);
    candidate_moves_ &= (~(1ULL << move));
    assert(((1ULL << move) & (player_ | opponent_)) == 0);
    flip = GetFlip(move, player_, opponent_);
  }
  if (flip != 0) {
    if (very_quick) {
      stats_->Add(1, QUICK1);
    } else {
      stats_->Add(1, QUICK2);
    }
  }
  return flip;
}

void MoveIteratorEval::Setup(
    BitPattern player, BitPattern opponent, BitPattern last_flip, int upper,
    HashMapEntry* const entry,
    EvaluatorDepthOneBase* evaluator_depth_one_base) {
  BitPattern empties = ~(player | opponent);
  BitPattern candidate_moves = Neighbors(opponent) & empties;
  BitPattern flip;
  remaining_moves_ = 0;
  empties_ = (Square) __builtin_popcountll(empties);
  if (entry && entry->best_move != kNoSquare) {
    assert(player == entry->player && opponent == entry->opponent);
    assert(entry->best_move == kNoSquare || (((1ULL << entry->best_move) & (player | opponent)) == 0));
    assert(entry->best_move == kNoSquare || GetFlip(entry->best_move, player, opponent) != 0);
  }
  depth_one_evaluator_ = evaluator_depth_one_base;
  FOR_EACH_SET_BIT(candidate_moves, square_pattern) {
    auto square = (Square) __builtin_ctzll(square_pattern);
    assert(((1ULL << square) & (player | opponent)) == 0);
    flip = GetFlip(square, player, opponent);
    if (flip == 0) {
      continue;
    }
    int value;
    if (entry && square == entry->best_move) {
      value = 99999999;
    } else {
      value = Eval(player, opponent, flip, upper, square, empties_);
    }
    moves_[remaining_moves_].Set(flip, value);
    remaining_moves_++;
  }
}

BitPattern MoveIteratorEval::NextFlip() {
  if (remaining_moves_ == 0) {
    return 0;
  }
  stats_->Add(1, MOVES_OR_DISPROOF);
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

int MoveIteratorMinimizeOpponentMoves::Eval(
    BitPattern player, BitPattern opponent, BitPattern flip, int upper,
    Square square, Square empties) {
  BitPattern moves = GetMoves(NewPlayer(flip, opponent), NewOpponent(flip, player));
  return
      -((int) __builtin_popcountll(moves) + (int) __builtin_popcountll(moves & kCornerPattern)) * 1000
      + kSquareValue[square];
}

int MoveIteratorDisproofNumber::Eval(
    BitPattern player, BitPattern opponent, BitPattern flip, int upper,
    Square square, Square empties) {
  BitPattern square_pattern = 1ULL << square;
  depth_one_evaluator_->Update(square_pattern, flip);
  EvalLarge eval = depth_one_evaluator_->Evaluate();
  int result = -DisproofNumberOverProb(NewPlayer(flip, opponent), NewOpponent(flip, player), -upper, eval);
  depth_one_evaluator_->UndoUpdate(square_pattern, flip);
  return result;
}

const EvaluatorAlphaBeta::EvaluateInternalFunction
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
const EvaluatorAlphaBeta::EvaluateInternalFunction
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

constexpr bool UpdateDepthOneEvaluator(int depth, bool solve) {
  return !solve || depth >= kMinEmptiesForDisproofNumber;
}

constexpr bool UseHashMap(int depth, bool solve) {
  return (solve && depth >= kMinEmptiesForHashMap)
          || (!solve && depth >= kMinDepthForHashMap);
}

EvaluatorAlphaBeta::EvaluatorAlphaBeta(
    HashMap<kBitHashMap>* hash_map,
    const EvaluatorFactory& evaluator_depth_one_factory) :
    hash_map_(hash_map),
    evaluator_depth_one_(evaluator_depth_one_factory()),
    stats_() {
  for (int depth = 0; depth < 64; ++depth) {
    for (bool solve : {true, false}) {
      for (bool unlikely : {true, false}) {
        int offset = MoveIteratorOffset(depth, solve, unlikely);
        if (unlikely) {
          if (depth <= 9) {
            move_iterators_[offset] = std::make_unique<MoveIteratorVeryQuick>(&stats_);
          } else {
            move_iterators_[offset] = std::make_unique<MoveIteratorQuick<true>>(&stats_);
          }
        } else if ((solve && depth <= 8) || (!solve && depth <= 2)) {
          move_iterators_[offset] = std::make_unique<MoveIteratorQuick<true>>(&stats_);
        } else if ((solve && depth <= 9) || (!solve && depth <= 4)) {
          move_iterators_[offset] = std::make_unique<MoveIteratorQuick<false>>(&stats_);
        } else if ((solve && depth < kMinEmptiesForDisproofNumber) || (!solve)) {
          move_iterators_[offset] = std::make_unique<MoveIteratorMinimizeOpponentMoves>(&stats_);
        } else {
          assert(UpdateDepthOneEvaluator(depth, solve));
          move_iterators_[offset] = std::make_unique<MoveIteratorDisproofNumber>(&stats_);
        }
      }
    }
  }
}

constexpr bool UseStabilityCutoff(int depth) {
  return depth > 3;
}

int EvaluatorAlphaBeta::VisitedToDisprove(const BitPattern player, const BitPattern opponent, const EvalLarge upper) {
  int to_be_visited = 0;
  BitPattern flip;
  BitPattern empties = ~(player | opponent);
  BitPattern candidate_moves = Neighbors(opponent) & empties;
  FOR_EACH_SET_BIT(candidate_moves, square_pattern) {
    auto square = (Square) __builtin_ctzll(square_pattern);
    BitPattern pattern = 1ULL << square;
    assert(((1ULL << square) & (player | opponent)) == 0);
    flip = GetFlip(square, player, opponent);
    if (flip == 0) {
      continue;
    }
    BitPattern new_player = NewPlayer(flip, opponent);
    BitPattern new_opponent = NewOpponent(flip, player);
    evaluator_depth_one_->Update(pattern, flip);
    to_be_visited += VisitedToProve(new_player, new_opponent, -upper);
    evaluator_depth_one_->UndoUpdate(pattern, flip);
  }
  return to_be_visited;
}

int EvaluatorAlphaBeta::VisitedToProve(const BitPattern player, const BitPattern opponent, const EvalLarge lower) {
  return (int) ByteToProofNumber(ProofNumber(player, opponent, lower, evaluator_depth_one_->Evaluate()));
}

template<int depth, bool passed, bool solve>
EvalLarge EvaluatorAlphaBeta::EvaluateInternal(
    const BitPattern player, const BitPattern opponent,
    const EvalLarge lower, const EvalLarge upper,
    const BitPattern last_flip, const BitPattern stable, int max_visited) {
  assert(kMinEvalLarge <= lower && lower < kMaxEvalLarge);
  assert(kMinEvalLarge < upper && upper <= kMaxEvalLarge);

  EvalLarge best_eval = kLessThenMinEvalLarge;
  Square best_move = kNoSquare;
  EvalLarge second_best_eval = kLessThenMinEvalLarge;
  Square second_best_move = kNoSquare;
  BitPattern new_stable = stable;
  EvalLarge stability_cutoff_upper = upper;
  if (UseStabilityCutoff(depth)) {
    new_stable = GetStableDisks(opponent, player, new_stable);
    stability_cutoff_upper = EvalToEvalLarge(GetUpperBoundFromStable(new_stable, opponent));
    if (stability_cutoff_upper <= lower) {
      return stability_cutoff_upper;
    }
  }

  std::unique_ptr<HashMapEntry> hash_entry = nullptr;
  if (UseHashMap(depth, solve)) {
    hash_entry = hash_map_->Get(player, opponent);
    if (hash_entry && hash_entry->depth >= depth) {
      if (hash_entry->lower >= upper || hash_entry->lower == hash_entry->upper) {
        return hash_entry->lower;
      } else if (hash_entry->upper <= lower) {
        return hash_entry->upper;
      }
    }
  }
  EvalLarge depth_zero_eval = lower;
  if (UpdateDepthOneEvaluator(depth, solve)) {
    depth_zero_eval = evaluator_depth_one_->Evaluate();
    evaluator_depth_one_->Invert();
  }
  bool unlikely = stability_cutoff_upper < lower + 120 || depth_zero_eval < lower - 40;
  MoveIteratorBase* moves =
      move_iterators_[MoveIteratorOffset(depth, solve, unlikely && depth <= 13)].get();
  moves->Setup(player, opponent, last_flip, upper, hash_entry.get(), evaluator_depth_one_.get());
  double to_be_visited = 0;
  double already_visited = (double) stats_.GetAll();
  bool try_early_filter = depth > 13 && solve && depth_zero_eval < upper - 32;
  if (try_early_filter) {
    assert(UpdateDepthOneEvaluator(depth, solve));
    to_be_visited = VisitedToDisprove(player, opponent, upper);
  }
  if (solve && to_be_visited + already_visited > max_visited) {
    return kLessThenMinEvalLarge;
  }
  BitPattern square;
  int cur_n_visited;
  for (BitPattern flip = moves->NextFlip(); flip != 0; flip = moves->NextFlip()) {
    square = SquareFromFlip(flip, player, opponent);
    if (UpdateDepthOneEvaluator(depth, solve)) {
      evaluator_depth_one_->Update(square, flip);
    }
    int current_eval;
    if (depth == 6 && solve) {
      cur_n_visited = 0;
      current_eval = -EvalToEvalLarge(EvalFiveEmpties(
          NewPlayer(flip, opponent), NewOpponent(flip, player),
          EvalLargeToEvalLower(-upper),
          EvalLargeToEvalUpper(-std::max(lower, best_eval)),
          flip, new_stable, &cur_n_visited));
      stats_.Add(cur_n_visited, LAST_5);
    } else if (!solve && depth == 1) {
      current_eval = (depth_zero_eval * kWeightDepthZero - evaluator_depth_one_->Evaluate() * kWeightDepthOne) / (kWeightDepthZero + kWeightDepthOne);
    } else {
      int max_lower_eval = std::max(lower, best_eval);
      BitPattern new_player = NewPlayer(flip, opponent);
      BitPattern new_opponent = NewOpponent(flip, player);

      if (try_early_filter) {
        to_be_visited -= VisitedToProve(new_player, new_opponent, -upper);
        assert(to_be_visited >= 0);
      }
      current_eval = -EvaluateInternal<NextNEmpties(depth), false, solve>(
          new_player, new_opponent,
          -upper, -max_lower_eval, flip, new_stable, max_visited - (int) to_be_visited);
    }
    if (current_eval == -kLessThenMinEvalLarge) {
      return kLessThenMinEvalLarge;
    }
    if (current_eval > best_eval) {
      second_best_eval = best_eval;
      second_best_move = best_move;
      best_eval = current_eval;
      best_move = (Square) __builtin_ctzll(square);
    } else if (current_eval > second_best_eval) {
      second_best_eval = current_eval;
      second_best_move = (Square) __builtin_ctzll(square);
    }
    if (UpdateDepthOneEvaluator(depth, solve)) {
      evaluator_depth_one_->UndoUpdate(square, flip);
    }
    if (best_eval >= upper) {
      break;
    }
  }
  if (best_eval == kLessThenMinEvalLarge) {
    if (passed) {
      best_eval = EvalToEvalLarge(GetEvaluationGameOver(player, opponent));
    } else {
      stats_.Add(1, PASS);
      best_eval = -EvaluateInternal<depth, true, solve>(
          opponent, player, -upper, -lower, last_flip, new_stable, max_visited);
      if (best_eval == -kLessThenMinEvalLarge) {
        return kLessThenMinEvalLarge;
      }
    }
  } else if (UseHashMap(depth, solve)) {
    hash_map_
        ->Update(player, opponent, depth, best_eval, lower, upper, best_move, second_best_move);
  }
  if (UpdateDepthOneEvaluator(depth, solve)) {
    evaluator_depth_one_->Invert();
  }
  return best_eval;
}
