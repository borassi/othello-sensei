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
#include "test_evaluator.h"

EvalLarge TestEvaluator::EvaluateInternal(BitPattern player, BitPattern opponent, int depth) {
  ++n_visited_;
  if (depth <= 0) {
    return depth_one_evaluator_->Evaluate();
  }
  std::vector<BitPattern> flips = GetAllMovesWithPass(player, opponent);

  if (flips.empty()) {
    return EvalToEvalLarge(GetEvaluationGameOver(player, opponent));
  }

  depth_one_evaluator_->Invert();
  EvalLarge best_eval = kMinEvalLarge;
  for (BitPattern flip : flips) {
    BitPattern square = SquareFromFlip(flip, player, opponent);
    if (flip != 0) {
      depth_one_evaluator_->Update(square, flip);
    }
    EvalLarge current_eval = -EvaluateInternal(
        NewPlayer(flip, opponent), NewOpponent(flip, player),
        depth - (flip == 0 ? 0 : 1));
    best_eval = std::max(best_eval, current_eval);
    if (flip != 0) {
      depth_one_evaluator_->UndoUpdate(square, flip);
    }
  }
  depth_one_evaluator_->Invert();
  if (depth == 1 && flips[0] != 0) {
    best_eval = (best_eval * kWeightDepthOne +
            depth_one_evaluator_->Evaluate() * kWeightDepthZero)
           / (kWeightDepthOne + kWeightDepthZero);
  }
  return best_eval;
}