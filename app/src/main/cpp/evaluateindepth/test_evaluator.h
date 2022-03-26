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

#include <math.h>
#include "../board/get_moves.h"
#include "../evaluatedepthone/evaluator_depth_one_base.h"
#include "../evaluatedepthone/test_evaluator_depth_one.h"

class TestEvaluator {
 public:
  TestEvaluator(EvaluatorDepthOneBase* depth_one_evaluator) :
      n_visited_(0), depth_one_evaluator_(depth_one_evaluator) {}

  NVisited GetNVisited() const { return n_visited_; }

  EvalLarge Evaluate(BitPattern player, BitPattern opponent, int depth) {
    n_visited_ = 0;
    return EvaluateInternal(player, opponent, depth);
  }
 private:

  EvalLarge EvaluateInternal(BitPattern player, BitPattern opponent, int depth) {
    ++n_visited_;
    if (depth <= 0) {
      depth_one_evaluator_->Setup(player, opponent);
      return depth_one_evaluator_->Evaluate();
    }
    std::vector<BitPattern> moves = GetAllMoves(player, opponent);

    if (moves.size() == 0) {
      return GetEvaluationGameOver(player, opponent);
    }

    EvalLarge best_eval = kMinEval * 8;
    for (BitPattern flip : moves) {
      EvalLarge current_eval = -EvaluateInternal(
          NewPlayer(flip, opponent), NewOpponent(flip, player), depth - 1);
      best_eval = std::max(best_eval, current_eval);
    }
    if (depth == 1) {
      depth_one_evaluator_->Setup(player, opponent);
      return (best_eval * kWeightDepthOne +
              depth_one_evaluator_->Evaluate() * kWeightDepthZero)
             / (kWeightDepthOne + kWeightDepthZero);
    }
    return best_eval;
  }

  NVisited n_visited_;
  EvaluatorDepthOneBase* depth_one_evaluator_;
};
